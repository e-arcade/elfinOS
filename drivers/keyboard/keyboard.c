#include "keyboard.h"
#include "../port.h"
#include "../../lib/io/io.h"
#include "../../lib/mem/mem.h"

static const char sc_ascii[] = {
    '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
    'u', 'i', 'o', 'p', '[', ']', '\n', '?', 'a', 's', 'd', 'f', 'g',
    'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' ',
};

enum { keyboard_buf_capacity = 1024 };

char* keyboard_buf;
unsigned keyboard_buf_size;

void keyboard_handler(stack_state* state) {
  // 0x60 - default port for keyboard
  uint8_t code = port_byte_in(0x60);
  if (code < sizeof(sc_ascii)) {
    char c = sc_ascii[code];
    if (keyboard_buf_size < keyboard_buf_capacity) {
      keyboard_buf[keyboard_buf_size++] = c;
    }
    char str[] = {c, '\0'};
    printk(str);
  } 
}

void keyboard_init() {
  keyboard_buf = kmalloc(keyboard_buf_capacity);
  
  add_interrupt_handler(IRQ1, keyboard_handler);
}
