#include "idt.h"
#include "../../lib/io/io.h"
#include "../../drivers/port.h"

gate_descriptor idt[GATE_NUM];

// defined in vectors.S
extern const uint32_t interrupt_vectors[];

void add_gate_descriptor(uint8_t i, bool istrap, uint32_t vector, uint8_t dpl) {
  idt[i].low_offset = low_16(vector);
  idt[i].selector = 0x08;                       // STA_X GDT
  idt[i].reserved = 0;                          // reserved
  idt[i].type = istrap ? STS_TG32 : STS_IG32;   // iterrupt or trap gate type
  idt[i].zero = 0;                              // always 0
  idt[i].dpl = dpl;                             // CPU privilege level
  idt[i].p = 1;                                 // 1 for valid
  idt[i].high_offset = high_16(vector);
}

void init_idt() {
  if (interrupt_vectors[0] == 0) {
    panic("IVT can't be empty!");
  }
  for (int i = 0; i < GATE_NUM; i++) {
    add_gate_descriptor(i, 0, interrupt_vectors[i], 0);
  }
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
}
