#include "../lib/io/io.h"
#include "../lib/str/str.h"
#include "../cpu/gdt/gdt.h"
#include "../drivers/uart/uart.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/vga/vga.h"
#include "../cpu/idt/idt.h"
#include "../fs/fs.h"
#include "proc.h"

void _start() {
  load_gdt();
  uart_init();
  keyboard_init();
  load_idt();
  __asm__("sti");
  
  vga_clear_screen();
  printk("\n                       == elfin kernel entry point ==\n", 's');
  
  printk("\n>> ", 's');
  while (1) {
    if (keyboard_buf_size > 0 && keyboard_buf[keyboard_buf_size - 1] == '\n') {
      keyboard_buf[keyboard_buf_size - 1] = '\0';

      if (keyboard_buf[0] != '\0') {
	// check if user wants to run a program
	if (!strncmp("run ", keyboard_buf, 4)) {
	  const char* program = keyboard_buf + 4;
	  run_elf(program);
	} else {
	  printk(keyboard_buf, 's');
	  printk(": command not found\n", 's');
	}	
      }
      
      keyboard_buf_size = 0;
      printk(">> ", 's');
    }

    // halt CPU until the next interrupt is fired
    __asm__("hlt");
  }
}
