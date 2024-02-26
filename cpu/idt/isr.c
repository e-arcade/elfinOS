#include "isr.h"
#include "idt.h"
#include "../../lib/io/io.h"
#include "../../drivers/port.h"
#include "../../kernel/syscall.h"
#include "../../kernel/proc.h"

static isr_t interrupt_handlers[GATE_NUM];

char* const exception_messages[] = {
    [0] = "Division By Zero",
    [1] = "Debug",
    [2] = "Non Maskable Interrupt",
    [3] = "Breakpoint",
    [4] = "Into Detected Overflow",
    [5] = "Out of Bounds",
    [6] = "Invalid Opcode",
    [7] = "No Coprocessor",

    [8] = "Double Fault",
    [9] = "Coprocessor Segment Overrun",
    [10] = "Bad TSS",
    [11] = "Segment Not Present",
    [12] = "Stack Fault",
    [13] = "General Protection Fault",
    [14] = "Page Fault",
    [15] = "Unknown Interrupt",

    [16] = "Coprocessor Fault",
    [17] = "Alignment Check",
    [18] = "Machine Check",
};

void interrupt_handler(stack_state* state) {
  uint8_t num = state->int_num;
    
  // send EOI (end of interrupt) to PIC
  if (num >= 40) {
    // follower
    port_byte_out(0xA0, 0x20);
  }
  if (num >= 32) {
    // leader
    port_byte_out(0x20, 0x20);
  }
  
  // handle interrupt
  if (interrupt_handlers[num] != 0) {
    isr_t handler = interrupt_handlers[num];
    handler(state);
    return;
  }
  
  // handle trap
  if (num < 32) {
    char* msg = "reserved";
    if (num < ARRLEN(exception_messages)) {
      msg = exception_messages[num];
    }
    panic(msg);
  }
}

void syscall_handler(stack_state* state) {
  // get handler result
  switch(state->eax) {
  case SYS_exit:
    if (state->ebx == 0) {
      printk("* success\n", 's');
    } else {
      printk("* failure\n", 's');
    }
    killproc();
  default:
    printk("unknown syscall!", 's');
    state->eax = -1;
  }
}

void add_interrupt_handler(uint8_t i, isr_t handler) {
  interrupt_handlers[i] = handler;
}
