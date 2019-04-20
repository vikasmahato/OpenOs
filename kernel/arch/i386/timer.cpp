#include <asm.h>
#include <stdio.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/constants.h>

int Timer::timer_ticks = 0;


Timer::Timer() {
    irq_install_handler(0, timer_handler);
    timer_phase(TICKS_PER_SECOND);
    printf("Timer installed.\n");
}

void Timer::timer_phase(int hz) {
    int divisor = 1193180 / hz;   // Calculates the divisor
    outb(0x43, 0x36);             // Set our command byte 0x36
    outb(0x40, divisor & 0xFF);   // Set low byte of divisor
    outb(0x40, divisor >> 8);     // Set high byte of divisor
}

void Timer::timer_handler(struct regs *r) {
    timer_ticks++;
}

