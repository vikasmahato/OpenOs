#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    SHORT_SHORT,
    SHORT,
    DEFAULT,
    LONG,
    LONG_LONG} length_specifier;

#define print_int(__type, __base, __parameters) {      \
  char buffer[20];                                     \
  int count = 0;                                       \
  __type num = va_arg(__parameters, __type);           \
  while (num != 0) {                                   \
    buffer[count++] = digits[num % __base];            \
    num /= base;                                       \
  }                                                    \
  for (size_t i = count; i != 0; i--)                  \
    putchar(buffer[i - 1]);                            \
};

const char digits[] = "0123456789abcdef";

static void print(const char* data, size_t data_length) {
    for (size_t i = 0; i < data_length; i++)
        putchar((int) ((const unsigned char*) data)[i]);
}

int printf(const char* restrict format, ...) {
    char cur_specifier;
    length_specifier length;
    bool is_signed;
    bool is_number;
    int base;

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

        cur_specifier = *++format;
        length = DEFAULT;
        is_number = true;
        is_signed = true;
        base = 10;

        // Deals with the length specifiers
        switch (cur_specifier) {
            case 'l':
                ++format;
                if (*format == 'l') {
                    length = LONG_LONG;
                    ++format;
                }
                else
                    length = LONG;
                break;
            case 'h':
                if (*(format + 1) == 'l') {
                    length = SHORT_SHORT;
                    ++format;
                }
                else
                    length = SHORT;
                break;
        }

        cur_specifier = *format;

        // Deals with the general specifiers
        switch (cur_specifier) {
            case 'c': ;
                const char c = (char) va_arg(parameters, int /* char promotes to int */);
                print(&c, 1);
                is_number = false;
                break;
            case 's': ;
                const char* s = va_arg(parameters, const char*);
                print(s, strlen(s));
                is_number = false;
                break;
            case 'd':
            case 'i': ;
                // default values is_signed and base
                break;
            case 'f':
            case 'F':
                break;
            case 'u':
                is_signed = false;
                break;
            case 'o':
                is_signed = false;
                base = 8;
                break;
            case 'x': ;
                is_signed = false;
                base = 16;
                print("0x", 2);
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

        if (is_number) {
            switch (length) {
                // SHORT won't be able to use my macro apparently :/
                // So, for now, no SHORT support
                case SHORT_SHORT:
                    break;
                case SHORT:
                    break;
                case DEFAULT:
                    if (is_signed) {
                        print_int(int, base, parameters);
                    } else {
                        print_int(unsigned int, base, parameters);
                    }
                    break;
                case LONG:
                    if (is_signed) {
                        print_int(long int, base, parameters);
                    } else {
                        print_int(unsigned long int, base, parameters);
                    }
                    break;
                case LONG_LONG:
                    if (is_signed) {
                        print_int(long long int, base, parameters);
                    } else {
                        print_int(unsigned long long int, base, parameters);
                    }
                    break;
            }
        }

        format++;
    }

    if (amount > 0) {
        print(format - amount, amount);
    }

    va_end(parameters);

    return written;
}