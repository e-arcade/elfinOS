#ifndef DRIVERS_KEYBOARD_KEYBOARD_H
#define DRIVERS_KEYBOARD_KEYBOARD_H

#include "../../cpu/idt/isr.h"

extern char* keyboard_buf;
extern unsigned keyboard_buf_size;

void keyboard_init();

#endif /* DRIVERS_KEYBOARD_KEYBOARD_H */
