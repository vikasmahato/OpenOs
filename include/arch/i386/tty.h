#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

void terminal_initialize(void);
void t_backspace();
void t_putchar(char c);
void t_write(const char* data, size_t size);
void t_writestring(const char* data);

#endif  // _KERNEL_TTY_H
