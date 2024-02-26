.global ret_interrupt_handler

// create stack state and go to main handler
init_interrupt_handler:
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushal

	// reload data segment register
	mov $0x10, %ax
	mov %ax, %ds

	pushl %esp
	call interrupt_handler
	add $0x4, %esp

ret_interrupt_handler:
	popal
	popl %gs
	popl %fs
	popl %es
	popl %ds
	// int_num, err_code
	addl $0x8, %esp
	// restore cs, eip, eflags, useresp, ss
	iret	

// each interrupt vector handle interrupt request and call isr
.macro create_vector i
vector\i:
	// check for the error code necessity
	// if not just push 0
	.if (!(\i == 8 || (\i >= 10 && \i <= 14) || \i == 17))
	pushl $0
	.endif
	
	pushl $\i
	jmp init_interrupt_handler
.endm

.altmacro
.macro init_ivt num
	.section .text
	create_vector \num
	
	.section .rodata
	.long vector\num
.endm

.section .rodata
.global interrupt_vectors
interrupt_vectors:
.set i, 0
.rept 256
	init_ivt %i
	.set i, i+1
.endr	
