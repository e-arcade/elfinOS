#include "io.h"

void panic(char* msg) {
  printk("\nKernel panic: ", 's');
  printk(msg, 's');
  asm("cli");
  while (1) {
    asm("hlt");
  }
}
