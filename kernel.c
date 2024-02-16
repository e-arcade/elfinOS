#include "console.h"
#include "drivers/vga.h"
#include "drivers/uart.h"


void _start() {
    uartinit();

    vga_clear_screen();
    printk("\n== elfin kernel entry point ==\n");
    asm("hlt");
}
