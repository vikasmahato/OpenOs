#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const char digits[] = "0123456789ABCDEF";

static void print(const char* data, size_t data_length) {
  for (size_t i = 0; i < data_length; i++)
    putchar((int) ((const unsigned char*) data)[i]);
}

static void print_num(unsigned int num, int base) {
  char buffer[20];
  int count = 0;
    while (num != 0) {
      buffer[count++] = digits[num % base];
      num /= base;
    }

    for (size_t i = count; i != 0; i--) {
      putchar(buffer[i - 1]);
  }
}

int printf(const char* restrict format, ...) {
  va_list parameters;
  va_start(parameters, format);

  int written = 0;
  size_t amount = 0;

  while (*format != '\0') {
    if (*format != '%') {
      amount += 1;
      written += 1;
      format++;
      continue;
    }

    if (amount > 0) {
      print(format - amount, amount);
      amount = 0;
    }

    char specified = *++format;

    switch (specified) {
      case 'c': ;
        const char c = (char) va_arg(parameters, int /* char promotes to int */);
        print(&c, 1);
        break;
      case 's': ;
        const char* s = va_arg(parameters, const char*);
        print(s, strlen(s));
        break;
      case 'd':
      case 'i': ;
        const int d = va_arg(parameters, int);
        print_num(d, 10);
        break;
      case 'f':
      case 'F':
        break;
      case 'u':
        break;
      case 'o':
        break;
      case 'x':
        break;
      case 'X':
        break;
      case 'a':
        break;
      case 'A':
        break;
      case 'p':
        break;
      case 'n':
        break;
      case '%':
        break;
      default:
        break;
    }

    format++;
  }

  if (amount > 0) {
    print(format - amount, amount);
  }

  va_end(parameters);

  return written;
}
