#include "io.h"
#include "../../drivers/vga/vga.h"
#include "../../drivers/uart/uart.h"

void printk(const char* msg) {
    vga_print_string_noscroll(msg);
    for (; *msg; ++msg) {
        uart_putc(*msg);
    }
}
