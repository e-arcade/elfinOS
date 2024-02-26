	// void swtch(void** old_stack, void* new_stack)

	.global swtch
swtch:
	mov 4(%esp), %eax // eax holds old_stack
	mov 8(%esp), %ecx // ecx holds new_stack

	// push old_stack callee-saved registers
	push %ebx
	push %ebp
	push %esi
	push %edi

	mov %esp, (%eax)  // update old_stack stack pointer
	mov %ecx, %esp    // get into a new stack

	// pop ukstack 'context' registers
	pop %edi
	pop %esi
	pop %ebp
	pop %ebx

	// now we have return address of 'ret_inrrupt_handler'
	// function on the top of the new_stack so
	// we can just use 'ret' instruction
	ret
	
