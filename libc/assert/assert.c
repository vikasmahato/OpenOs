#include <assert.h>
#include <stdio.h>

void panic_assert(const char *file, unsigned int line, const char *desc)
{
    // An assertion failed, and we have to panic.
    asm volatile("cli"); // Disable interrupts.

    printf("ASSERTION-FAILED(");
    printf(desc);
    printf(") at ");
    printf(file);
    printf(":");
    printf("%u", line);
    printf("\n");
    // Halt by going into an infinite loop.
    for(;;);
}