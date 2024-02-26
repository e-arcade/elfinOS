#include "io.h"
#include "../../drivers/vga/vga.h"
#include "../../drivers/uart/uart.h"

#include <stdint.h>
#include <stddef.h>

size_t get_num_len(int num) {
  if (num == 0) return 1;
  size_t len = 0;
  while (num != 0) {
    num /= 10;
    len++;
  }

  return len;
}

char* int_to_str(int* num) {
  char buf[1024];
  size_t i = get_num_len(*num);
  buf[i--] = '\0';

  if (*num == 0) {
    buf[i] = '0';
  } else {
    while (*num != 0) {
      buf[i] = *num % 10 + '0';
      *num /= 10;
      i--;
    }
  }
  char* res = buf;
  
  return res;
}

void printk(void* msg, char spec) {
  vga_print_string_noscroll(msg);
  char* res;
  switch(spec) {
  case 'd':
    res = int_to_str((int*)msg);
    break;
  case 's':
    res = (char*)msg;
    break;
  case 'h':
    break;
  default:
    break;
  }

  for (; *res; ++res) {
    uart_putc(*res);
  }
}
