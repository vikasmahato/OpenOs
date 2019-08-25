#include <libk/basesystem.h>

#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/tty.h>
#include <asm.h>
#include <devices/kb.h>
#include <devices/timer.h>
#include <external/multiboot.h>
#include <libk/phys_mem.h>
#include <libk/virt_mem.h>
#include <libk/heap_mem.h>

#include <stdio.h>

void BaseSystem::init(multiboot_info* mb) {
    terminal_initialize();
    print_early_boot_info(mb);
    init_gdt();
    init_idt();
    init_isr();
    init_irq();
    PhysicalMemoryManager physicalMemoryManager(mb);
    VirtualMemoryManager virtualMemoryManager(&physicalMemoryManager);
    HeapMemoryManager heapMemoryManager(&virtualMemoryManager, HEAP_VIRT_ADDR_START, HEAP_VIRT_ADDR_START+HEAP_INITIAL_BLOCK_SIZE, 0xCFFFF000, false, false);
    DriverManager driverManager;
    initializeDrivers(&driverManager);
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
    //isrs_install();
    return true;
}

bool BaseSystem::init_irq() {
    //irq_install();
    return true;
}

bool BaseSystem::initializeDrivers(DriverManager* driverManager) {
    Timer timer;
    Keyboard keyboard;

    driverManager->addDriver(&timer);
    driverManager->addDriver(&keyboard);
    driverManager->initializeAll();
    return true;
}