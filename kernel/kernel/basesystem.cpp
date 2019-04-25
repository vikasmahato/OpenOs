#include <kernel/basesystem.h>

#include <asm.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/isrs.h>
#include <kernel/timer.h>
#include <kernel/tty.h>
#include <kernel/kb.h>


void BaseSystem::init() {
    terminal_initialize();
    init_gdt();
    init_idt();
    init_isr();
    init_irq();
    init_timer();
    init_keyboard();
    enable_interrupts();
}

void BaseSystem::print_early_boot_info() {}

bool BaseSystem::init_gdt() {
    gdt_install();
    return true;
}

bool BaseSystem::init_idt() {
    idt_install();
    return true;
}

bool BaseSystem::init_isr() {
    isrs_install();
    return true;
}

bool BaseSystem::init_irq() {
    irq_install();
    return true;
}

bool BaseSystem::init_timer() {
    Timer timer;
    return true;
}

bool BaseSystem::init_keyboard() {
    Keyboard keyboard;
    return true;
}