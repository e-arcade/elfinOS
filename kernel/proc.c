#include "proc.h"
#include "elf.h"
#include "../lib/io/io.h"
#include "../lib/mem/mem.h"
#include "../lib/str/str.h"
#include "../fs/fs.h"

void ret_interrupt_handler();
void swtch(void** oldstack, void* newstack);

_vm* vm;

void run_elf(const char* name) {
  (void)name;
  if (!vm) {
    vm = kmalloc(sizeof(_vm));
    vm->user_task = kmalloc(sizeof(task));
    // setup tss for ukstack
    install_tss(&vm->user_task->tss, vm->user_task->stack.bottom);
  }
  
  if (read_file(name, (void*)USER_BASE, 100 << 20) <= 0) {
    printk((char*)name, 's');
    printk(": file not found!\n", 's');
    return;
  }

  elf32_hdr* elf = (void*)USER_BASE;
  
  ukstack* uk = &vm->user_task->stack;
  memset(uk, 0, sizeof(*uk));
  
  uk->context.ra = (uint32_t)ret_interrupt_handler;

  stack_state* frame = &uk->frame;
  frame->eip = elf->e_entry;
  frame->cs = (SEG_UCODE << 3) | DPL_USER;
  frame->ds = (SEG_UDATA << 3) | DPL_USER;
  frame->es = frame->ds;
  frame->ss = frame->ds;
  frame->fs = frame->ds;
  frame->gs = frame->ds;
  frame->eflags = FL_IF;
  frame->useresp = USER_STACK_BASE;

  swtch(&vm->kernel_thread, &uk->context);
}

_Noreturn void killproc() {
    void* task_stack;
    swtch(&task_stack, vm->kernel_thread);
    __builtin_unreachable();
}
