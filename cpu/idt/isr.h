#ifndef CPU_IDT_ISR_H
#define CPU_IDT_ISR_H

#include <stdint.h>

enum {
  IRQ0 = 32,
  IRQ1,
  IRQ2,
  IRQ3,
  IRQ4,
  IRQ5,
  IRQ6,
  IRQ7,
  IRQ8,
  IRQ9,
  IRQ10,
  IRQ11,
  IRQ12,
  IRQ13,
  IRQ14,
  IRQ15
};

typedef struct {
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // pusha
  uint32_t gs, fs, es, ds;
  uint32_t int_num, error_code; // interrupt number + error code
  uint32_t eip, cs, eflags;     // pushed by the CPU by default

  uint32_t useresp, ss;         // Pushed by the CPU for userspace interrupts
} stack_state;

#define ARRLEN(a) (sizeof(a)/sizeof(a[0]))

typedef void (*isr_t)(stack_state*);

void interrupt_handler(stack_state* state);
void add_interrupt_handler(uint8_t i, isr_t handler);

#endif /* CPU_IDT_ISR_H */
