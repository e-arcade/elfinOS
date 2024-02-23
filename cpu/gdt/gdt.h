#ifndef CPU_GDT_GDT_H
#define CPU_GDT_GDT_H

#define STA_X     0x8     // Executable segment
#define STA_W     0x2     // Writeable (non-executable segments)
#define STA_R     0x2     // Readable (executable segments)

// System segment type bits
#define STS_T32A  0x9     // Available 32-bit TSS
#define STS_IG32  0xE     // 32-bit Interrupt Gate
#define STS_TG32  0xF     // 32-bit Trap Gate

#define DPL_USER  3

#define FL_IF     0x00000200

#define SEG_KCODE 1       // kernel code
#define SEG_KDATA 2       // kernel data + stack
#define SEG_UCODE 3       // user code
#define SEG_UDATA 4       // user data + stack
#define SEG_TSS   5       // this process's task state

// -------------------------------------------------
// GDT KERNEL CODE SEGMENT DESCRIPTOR:
//
// limit[15: 0] - 1111111111111111
// base[15: 0]  - 0000000000000000
// base[23:16]  - 00000000
//
// access byte - 10011010
// 1  - present bit (always 0)
// 00 - highest privilege level (kernel)
// 1  - code or data segment
// 1  - executable
// 0  - only kernel can execute
// 1  - readable
// 0  - CPU will set it when the segment is accessed
//
// limit[19:16] - 1111
//
// flags - 1100
// 1  - page granularity (limit is in 4KiB blocks)
// 1  - descriptor defines 32-bit segment
// 0  - cleared for 32-bit segment
// 0  - reserved
//
// base[31:24] - 00000000
//
// -------------------------------------------------
// GDT KERNEL DATA SEGMENT DESCRIPTOR
//
// limit[15: 0] - 1111111111111111
// base[15: 0]  - 0000000000000000
// base[23:16]  - 00000000
//
// access byte - 10010010
// 1  - present bit (always 0)
// 00 - highest privilege level (kernel)
// 1  - code or data segment
// 0  - not executable -> data segment
// 0  - segment grows up, offset less than limit
// 1  - readable
// 0  - CPU will set it when the segment is accessed
//
// limit[19:16] - 1111
//
// flags - 1100
// 1  - page granularity (limit is in 4KiB blocks)
// 1  - descriptor defines 32-bit segment
// 0  - cleared for 32-bit segment
// 0  - reserved
//
// base[31:24] - 00000000 

#define USER_BASE       0x400000  // 4 MB
#define USER_STACK_BASE 0xf00000  // 15 MB
#define KERN_STACK_BASE 0x090000

// The 0xC0 means the limit is in 4096-byte units
// and (for executable segments) 32-bit mode.
#define SEG_ASM(type,base,lim)                                  \
  .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);		\
  .byte (((base) >> 16) & 0xff), (0x90 | (type)),		\
    (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef struct {
  uint16_t lim_15_0;            // low bits of segment limit
  uint16_t base_15_0;           // low bits of segment base address
  uint8_t  base_23_16;          // middle bits of segment base address
  uint8_t  type      : 4;       // segment type (STA_ constants)
  uint8_t  s         : 1;       // 0 = system, 1 = code/data 
  uint8_t  dpl       : 2;       // descriptor Privilege Level
  uint8_t  p         : 1;       // present
  uint8_t  lim_19_16 : 4;       // high bits of segment limit
  uint8_t  avl       : 1;       // unused (available for software use)
  uint8_t  rsv1      : 1;       // reserved
  uint8_t  db        : 1;       // 0 = 16-bit segment, 1 = 32-bit segment
  uint8_t  g         : 1;       // granularity: limit scaled by 4K when set
  uint8_t  base_31_24;          // high bits of segment base address
} __attribute__((packed)) seg_descriptor;

typedef struct {
  uint16_t size;
  void* offset;
} __attribute__((packed)) gdt_descriptor;

// normal segment
#define SEG(type, base, lim, dpl) (seg_descriptor)			\
  { ((lim) >> 12) & 0xffff, (uint32_t)(base) & 0xffff,			\
      ((uint32_t)(base) >> 16) & 0xff, type, 1, dpl, 1,			\
      (uint32_t)(lim) >> 28, 0, 0, 1, 1, (uint32_t)(base) >> 24 }

// segment for TSS
#define SEG16(type, base, lim, dpl) (seg_descriptor)			\
  { (lim) & 0xffff, (uint32_t)(base) & 0xffff,				\
      ((uint32_t)(base) >> 16) & 0xff, type, 1, dpl, 1,			\
      (uint32_t)(lim) >> 16, 0, 0, 1, 0, (uint32_t)(base) >> 24 }

typedef struct {
  uint32_t link;                // old ts selector
  uint32_t esp0;                // stack pointers and segment selectors
  uint16_t ss0;                 // after an increase in privilege level
  uint16_t padding1;         
  uint32_t esp1;         
  uint16_t ss1;         
  uint16_t padding2;         
  uint32_t esp2;         
  uint16_t ss2;         
  uint16_t padding3;         
  void     *cr3;                // page directory base
  uint32_t *eip;                // saved state from last task switch
  uint32_t eflags;         
  uint32_t eax;                 // more saved state (registers)
  uint32_t ecx;         
  uint32_t edx;         
  uint32_t ebx;         
  uint32_t *esp;         
  uint32_t *ebp;         
  uint32_t esi;         
  uint32_t edi;         
  uint16_t es;                  // even more saved state (segment selectors)
  uint16_t padding4;         
  uint16_t cs;         
  uint16_t padding5;         
  uint16_t ss;         
  uint16_t padding6;         
  uint16_t ds;         
  uint16_t padding7;         
  uint16_t fs;         
  uint16_t padding8;         
  uint16_t gs;         
  uint16_t padding9;         
  uint16_t ldt;         
  uint16_t padding10;         
  uint16_t t;                   // trap on task switch
  uint16_t iomb;                // I/O map base address
} task_state;

void init_gdt();
void load_gdt();
void install_tss(task_state* tss, void* esp);
#endif

#endif /* CPU_GDT_GDT_H */
