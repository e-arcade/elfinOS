#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H

enum {
  SYSCALL_CODE=0x80,
  SYS_exit=0,
};

int syscall(int call, int arg);

#endif /* KERNEL_SYSCALL_H */
