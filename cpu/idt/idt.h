#ifndef CPU_IDT_IDT_H
#define CPU_IDT_IDT_H

#include <stdint.h>
#include <stdbool.h>

#define GATE_NUM 256

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

#define STS_IG32 0xE       // 32-bit Interrupt Gate
#define STS_TG32 0xF       // 32-bit Trap Gate

typedef struct {
  uint16_t low_offset;     // low part of the ISR address
  uint16_t selector;       // point to a valid code seg in GDT
  uint8_t  reserved;       // reserved
  uint8_t  type : 4;       // define the type of the gate
                           // 0b0101 (0x5) - Task Gate (offset 0)
                           // 0b0110 (0x6) - 16-bit Interrupt Gate
                           // 0b0111 (0x7) - 16-bit Trap Gate
                           // 0b1110 (0xE) - 32-bit Interrupt Gate
                           // 0b1111 (0xF) - 32-bit Trap Gate
  uint8_t  zero : 1;       // always 0
  uint8_t  dpl  : 2;       // CPU privilege level (use INT)
  uint8_t  p    : 1;       // set fo the desriptor to be valid
  uint16_t high_offset;    // high part of the ISR address
} __attribute__((packed)) gate_descriptor;

// the location of the IDT is kept in the IDTR (IDT register)
typedef struct {
  uint16_t size;           // size of the IDT in bytes - 1
  void* offset;            // linear IDT address
} __attribute__((packed)) idt_descriptor;

void add_gate_descriptor(uint8_t i, bool istrap, uint32_t vector, uint8_t dpl);
void init_idt();
void load_idt();

#endif /* CPU_IDT_IDT_H */
