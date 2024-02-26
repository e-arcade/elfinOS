#ifndef KERNEL_PROC_H
#define KERNEL_PROC_H

#include "../cpu/idt/isr.h"
#include "../cpu/gdt/gdt.h"

#include <stdint.h>

typedef struct {
  uint32_t edi, esi, ebp, ebx;
  // return address
  uint32_t ra;
} context;

typedef struct {
  uint32_t space[400];
  context context;
  stack_state frame;
  char bottom[];
} ukstack;

typedef struct {
  task_state tss;
  ukstack stack;
} task;

typedef struct {
  void* kernel_thread;
  void* user_thread;
  task* user_task;
} _vm;

void run_elf(const char* name);
_Noreturn void killproc();

#endif /* KERNEL_PROC_H */
