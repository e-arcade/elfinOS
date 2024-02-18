#include "../cpu/gdt/gdt.h"

.code16
.global _start
_start:
	// get disk number from %dl
	mov %dl, boot_drive
	// move booting banner to %si
	// and print it
	mov $banner, %si
	call print_string

	call get_drive_params
	// load kernel code into 0x1000 memory address
	call load_kernel
	// we loaded 32-bit kernel -> switch to 32-bit mode
	call switch_to_32bit

	hlt // halt cpu
	jmp . // loop forever

// use 13h bios interrupt vector
// with 0x8 ah to get drive params	
get_drive_params:
	mov $8, %ah
	// get disk number
	mov boot_drive, %dl
	// create interrupt
	int $0x13
	inc %dh
	// get number of heads
	mov %dh, disk_heads
	// get sectors per track
	and 0x3f, %cl
	mov %cl, sectors_per_track
	ret

// ELF HEADER
.equ ELF32_ENTRY_OFFSET, 0x18 // 24 bytes offset
.equ ELF32_PHOFF_OFFSET, 0x1c // 28 bytes offset
.equ ELF32_PHENTSIZE_OFFSET, ELF32_PHOFF_OFFSET + 14 // 42 bytes offset
.equ ELF32_PHNUM_OFFSET, ELF32_PHENTSIZE_OFFSET + 2 // 44 bytes offset
	
// Program header table
.equ ELF32_PHDR_PTYPE_OFFSET, 0 // 0 bytes offset
.equ ELF32_PHDR_P_OFFSET, 0x4 // 4 bytes offset
.equ ELF32_PHDR_FILESZ_OFFSET, 0x10 // 16 bytes offset
	
// Kernel code offset
.equ KERNEL_OFFSET, 0x1000

// segment type
.equ PT_LOAD, 1

.equ MBR_SECTORS, 2
.equ SECTOR_BASE, 1
.equ ELFHDR_SECTORS, 8

.equ SECTOR_SIZE, 512
.equ SECTOR_SHIFT, 9

load_kernel:
	mov $1, %al // sectors to read
	mov $SECTOR_BASE + MBR_SECTORS, %cl // start after MBR

	// read first 512 bytes of the kernel ELF file
	// from the third disk sector into 0x1000 memory address
	call bios_disk_read
	
	// %si - kernel entry point address
	mov KERNEL_OFFSET + ELF32_ENTRY_OFFSET, %si
	mov %si, entry

	// get program headers count
	mov KERNEL_OFFSET + ELF32_PHNUM_OFFSET, %si
read_segment:	
	// not include program header segment for elf header
	// (segment 00)
	dec %si
	// ax - actual program headers count
	mov %si, %ax
	
	// ax - al * 32
	mulb KERNEL_OFFSET + ELF32_PHENTSIZE_OFFSET
	
	// di - size of actual program headers in bytes
	mov %ax, %di	
	// di - size of ELF header and actual program headers
	// in bytes
	add KERNEL_OFFSET + ELF32_PHOFF_OFFSET, %di
	
	// ax - type of segment (phentry)
	mov KERNEL_OFFSET + ELF32_PHDR_PTYPE_OFFSET(%di), %ax
	cmp $PT_LOAD, %ax
	jnz read_segment // not a PT_LOAD segment

	// ax - size of current segment in bytes
	mov KERNEL_OFFSET + ELF32_PHDR_FILESZ_OFFSET(%di), %ax
	// check if current segment is empty (size = 0)
	test %ax, %ax
	// if so, read the next segment
	jz read_segment

	// ax - current segment offset + current segment size
	add KERNEL_OFFSET + ELF32_PHDR_P_OFFSET(%di), %ax

	// subtract 8 sectors that the ELF header occupies
	sub $0x1000, %ax // we won't load the header
	// add one more sector so as not to lose the data
	// lying in the last sector
	add $SECTOR_SIZE - 1, %ax
	// divide by 512 to find rounded up sectors count
	// needed for the kernel program
	shr $SECTOR_SHIFT, %ax

	// get the sector number of the kernel entry
	mov $SECTOR_BASE + MBR_SECTORS + ELFHDR_SECTORS, %cl
	// read all kernel code into 0x1000 memory address
	call bios_disk_read
	ret
	
// %al - number of sectors to read
// %ch - cylinder number	
// %cl - current sector number
// %dh - head number	
// %dl - disk number
// %bx - pointer to where we want to load the sectors 	
bios_disk_read:
	xor %ah, %ah
	// %si - number of sectors to read
	mov %ax, %si

	// specify cylinder number
	mov $0, %ch
	// specify head number
	mov $0, %dh
	// destination pointer
	mov $KERNEL_OFFSET, %bx
	// specify disk number
	mov boot_drive, %dl
	// read 1 sector
	mov $1, %al

// use 13h bios interrupt vector
// with 0x2 ah to read sectors	
1:
	mov $2, %ah
	// create interrupt
	int $0x13
	// jump if carry flag
	jc fail
	// increase destination pointer by sector size
	add $SECTOR_SIZE, %bx
	// increment sector number
	inc %cl
	dec %si
	jnz 1b
	ret

fail:
	mov $read_error, %si
	call print_string
	hlt
	jmp .

switch_to_32bit:
	// enable A20
	mov $2, %al
	out %al, $0x92

	// disable interrupts
	cli
	// load DGT
	lgdt gdt_descriptor

	// enable protected mode
	mov %cr0, %eax
	or $1, %eax
	mov %eax, %cr0

	// cs - 0x1000
	// eip - init_32bit address
	ljmp $SEG_KCODE << 3, $init_32bit

.code32
init_32bit:
	// initialize all segments with
	// .data section address (0x2000)
	mov $SEG_KDATA << 3, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	// setup stack
	mov $KERN_STACK_BASE, %ebp
	mov %ebp, %esp

	// get kernel entry point address
	movzwl entry, %esi
	// jump to the kernel code
	call *%esi
	jmp .

.code16
// use 10h bios interrupt vector
// with 0xe ah for teletype output	
print_string:
	mov $0x0e, %ah
repeat:
	// mov (%si), %al; inc %si
	lodsb
	// check for '\0'
	test %al, %al
	je done
	// create interrupt
	int $0x10
	jmp repeat
done:
	ret

. = _start + 256     # pad to 256 bytes
	
boot_drive:
	.byte 0
banner:
	.asciz "elfin bootloader started..\n\r"
read_error:
	.asciz "read error\n\r"
	// aligning by 2 bytes
	.balign 2
entry:
	.word 0
disk_heads:
	.byte 0
sectors_per_track:
	.byte 0
	// aligning by 4 bytes
	.balign 4

gdt_start:
	.quad 0x0				      // null descriptor
	SEG_DESCRIPTOR(STA_X|STA_R, 0x0, 0xffffffff)  // code seg
	SEG_DESCRIPTOR(STA_W, 0x0, 0xffffffff)        // data seg
gdt_end:

// define global descriptor table
gdt_descriptor:
	.word gdt_end - gdt_start - 1                 // size (16 bit)
	.int  gdt_start                               // address (32 bit)

. = _start + 510     # pad to 510 bytes
.byte 0x55, 0xaa     # boot sector magic value
