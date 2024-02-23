#include "idt.h"
#include "isr.h"
#include "../gdt/gdt.h"
#include "../../lib/io/io.h"
#include "../../drivers/port.h"
#include "../../kernel/syscall.h"

gate_descriptor idt[GATE_NUM];

// defined in vectors.S
extern const uint32_t interrupt_vectors[];

// - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate.
//   interrupt gate clears FL_IF, trap gate leaves FL_IF alone
// - selector: code segment selector for interrupt/trap handler
// - low_offset/high_offset: offset in code segment for interrupt/trap handler
// - dpl: Descriptor Privilege Level -
//        the privilege level required for software to invoke
//        this interrupt/trap gate explicitly using an int instruction
void add_gate_descriptor(uint8_t i, bool istrap, uint32_t vector, uint8_t dpl) {
  idt[i].low_offset = low_16(vector);
  idt[i].selector = 0x08;
  idt[i].reserved = 0;
  // IG32 - interrupts for kernel
  // TG32 - traps (syscalls from userspace)
  idt[i].type = istrap ? STS_TG32 : STS_IG32;
  idt[i].zero = 0;
  // privilege level for input interrupt requests
  idt[i].dpl = dpl;
  idt[i].p = 1;
  idt[i].high_offset = high_16(vector);
}

void init_idt() {
  if (interrupt_vectors[0] == 0) {
    panic("IVT can't be empty!");
  }
  for (int i = 0; i < GATE_NUM; i++) {
    add_gate_descriptor(i, 0, interrupt_vectors[i], 0);
  }
  add_gate_descriptor(SYSCALL_CODE, 1, interrupt_vectors[SYSCALL_CODE], DPL_USER);
}

static void init_pic() {
    // ICW1
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);

    // ICW2
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);

    // ICW3
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);

    // ICW4
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);

    // OCW1
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);
}

void load_idt() {
  init_idt();

  idt_descriptor idtr;
  idtr.size = sizeof(idt) - 1;
  idtr.offset = &idt;
  
  __asm__("lidt (%0)" :: "r"(&idtr));

  init_pic();

  //add_interrupt_handler(SYSCALL_CODE, syscall_handler);
}
