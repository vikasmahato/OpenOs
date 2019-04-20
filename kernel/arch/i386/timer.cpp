#include <asm.h>
#include <stdio.h>
#include <kernel/idt.h>
#include <kernel/irq.h>

#define TICKS_PER_SECOND 100

#ifdef __cplusplus
extern "C"
{
#endif

// Holds how many ticks that the system has been running for
int timer_ticks = 0;

void timer_phase(int hz) {
    int divisor = 1193180 / hz;   // Calculates the divisor
    outb(0x43, 0x36);             // Set our command byte 0x36
    outb(0x40, divisor & 0xFF);   // Set low byte of divisor
    outb(0x40, divisor >> 8);     // Set high byte of divisor
}

// IRQ Handler for the timer. Called at every clock tick
void timer_handler(struct regs *r) {
    timer_ticks++;
}

// Sets up the system clock
void timer_install() {
    irq_install_handler(0, timer_handler);
    timer_phase(TICKS_PER_SECOND);
    printf("Timer installed.\n");
}

#ifdef __cplusplus
}
#endif