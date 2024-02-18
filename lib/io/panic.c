#include "io.h"

void panic(const char* msg) {
    printk("\nKernel panic: ");
    printk(msg);
    asm("cli");
    while (1) {
        asm("hlt");
    }
}
