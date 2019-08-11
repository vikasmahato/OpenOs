#include <arch/i386/idt.h>
#include <arch/i386/interrupts.h>
#include <asm.h>
#include <devices/timer.h>
#include <stdio.h>

#define TICKS_PER_SECOND 100

// Holds how many ticks that the system has been running for
int Timer::timer_ticks = 0;

// Sets up the system clock
Timer::Timer() {
    register_interrupt_handler(TIMER_IDT_INDEX, timer_handler);
    timer_phase(TICKS_PER_SECOND);
  printf("Timer installed.\n");
}

void Timer::timer_phase(int hz) {
    int divisor = 1193180 / hz;   // Calculates the divisor
    outb(0x43, 0x36);             // Set our command byte 0x36
    outb(0x40, divisor & 0xFF);   // Set low byte of divisor
    outb(0x40, divisor >> 8);     // Set high byte of divisor
}

void Timer::timer_handler(__attribute__((unused)) regs *r) {
    timer_ticks++;
}

void Timer::initialize() {}
void Timer::reset() {}
void Timer::destroy() {}