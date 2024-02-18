#include "isr.h"
#include "idt.h"
#include "../../lib/io/io.h"
#include "../../drivers/port.h"

static isr_t interrupt_handlers[GATE_NUM];

const char* const exception_messages[] = {
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
  
  // handle trap
  if (num < 32) {
    const char* msg = "reserved";
    if (num < ARRLEN(exception_messages)) {
      msg = exception_messages[num];
    }
    panic(msg);
  }
  // handle interrupt
  if (interrupt_handlers[num] != 0) {
    isr_t handler = interrupt_handlers[num];
    handler(state);
  }

  // EOI
  if (num >= 40) {
    port_byte_out(0xA0, 0x20); /* follower */
  }
  if (num >= 32) {
    port_byte_out(0x20, 0x20); /* leader */
  }
}

void add_interrupt_handler(uint8_t i, isr_t handler) {
  interrupt_handlers[i] = handler;
}
