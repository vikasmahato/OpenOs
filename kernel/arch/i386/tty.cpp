#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/vga.h>

#ifdef __cplusplus
extern "C"
{
#endif

size_t terminal_line_fill[VGA_WIDTH];
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_scroll();

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    terminal_buffer = VGA_MEMORY;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = make_vgaentry(' ', terminal_color);
        }
    }
}

void terminal_backspace() {
    if (terminal_column == 0) {
        if (terminal_row > 0) {
            terminal_row--;
        }
        terminal_column = terminal_line_fill[terminal_row];
    } else {
        terminal_column--;
    }

    terminal_putentryat(32, terminal_color, terminal_column, terminal_row);
    update_cursor(terminal_row, terminal_column);
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c) {
    if (c != '\n') {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    }

    if (++terminal_column == VGA_WIDTH || c == '\n') {
        terminal_line_fill[terminal_row] = terminal_column - 1;
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    }

    update_cursor(terminal_row, terminal_column);
}

void terminal_scroll() {
    terminal_row--;
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t src_index = y * VGA_WIDTH + x;
            const size_t dsterminal_index = (y + 1) * VGA_WIDTH + x;
            terminal_buffer[src_index] = terminal_buffer[dsterminal_index];
        }
        terminal_line_fill[y] = terminal_line_fill[y + 1];
    }

    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        terminal_buffer[index] = make_vgaentry(' ', terminal_color);
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

#ifdef __cplusplus
}
#endif