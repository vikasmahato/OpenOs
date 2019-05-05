#include <kernel/basesystem.h>

#include <asm.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/isrs.h>
#include <kernel/timer.h>
#include <kernel/tty.h>
#include <kernel/kb.h>
#include <kernel/phy_mem_manager.h>


void BaseSystem::init(multiboot_info* mb) {
    terminal_initialize();
    init_gdt();
    init_idt();
    init_isr();
    init_irq();
    PhysicalMemoryManager physicalMemoryManager(mb);
    init_timer();
    init_keyboard();
    enable_interrupts();
}

void BaseSystem::print_early_boot_info(multiboot_info* mb) {
    multiboot_memory_map_t* mm = (multiboot_memory_map_t*) mb->mmap_addr;
    while ((unsigned int) mm < mb->mmap_addr + mb->mmap_length) {
        if (mm->type != MULTIBOOT_MEMORY_AVAILABLE) {
            printf("Unavailable physical address: %llx, type: %u, length: %llx\n",
                   mm->addr, mm->type, mm->len);
        } else {
            printf("Available physical address: %llx, length: %llx\n",
                   mm->addr, mm->len);
        }
        mm = (multiboot_memory_map_t*) ((unsigned int) mm +
                                        mm->size + sizeof(mm->size));
    }
}

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