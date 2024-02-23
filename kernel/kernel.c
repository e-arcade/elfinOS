#include "../lib/io/io.h"
#include "../cpu/gdt/gdt.h"
#include "../drivers/uart/uart.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/vga/vga.h"
#include "../cpu/idt/idt.h"

void _start() {
  load_gdt();
  uart_init();
  keyboard_init();
  load_idt();
  __asm__("sti");

  vga_clear_screen();
    
  printk("\n== elfin kernel entry point ==\n");
  while (1) __asm__("hlt");
}
