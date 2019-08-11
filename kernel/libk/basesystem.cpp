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
#include <libk/heap.h>

#include <stdio.h>
Heap* kernelHeap;

void BaseSystem::init(multiboot_info* mb) {
    terminal_initialize();
    print_early_boot_info(mb);
    init_gdt();
    init_idt();
    init_isr();
    init_irq();
    PhysicalMemoryManager physicalMemoryManager(mb);
    VirtualMemoryManager virtualMemoryManager(&physicalMemoryManager);
    DriverManager driverManager;
    initializeDrivers(&driverManager);
    Heap heap(&virtualMemoryManager);
    kernelHeap = &heap;
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

// TODO: move to separate source
void* kmalloc(size_t bytes) {
    if(bytes == 0 || bytes > HEAP_BLOCK_SIZE * HEAP_BLOCK_COUNT) {
        return nullptr;
    }

    size_t numberOfBlocksToBeAllocated = HEAP_BLOCKS_NEED_FOR_N_BYTES(bytes);

    HeapPage* freeHeapPage = kernelHeap->getFittingHeapPage(numberOfBlocksToBeAllocated);
    
    /* If we cannot find a free heap page request more memory. */
    if(!freeHeapPage) {
        kernelHeap->requestMemory();
        return kmalloc(bytes); // ?Infinite recursion possible
    }

    /* Tries to fiend a sequence of blocks that can fit the bytes in the heap
     * page. If we can't find it, request a new block of 4KB and try mallocing it
     */
    int32_t firstFittingBlock = kernelHeap->findFittingBlockStart(freeHeapPage,
                                                         numberOfBlocksToBeAllocated);
    if (firstFittingBlock == -1) {
        kernelHeap->requestMemory();
        return kmalloc(bytes); // ?Infinite recursion possible
    }

    /* Success! Populate the bitmaps in the heap page to indicate that we 
     * allocated the memory */
    kernelHeap->allocateBlocks(freeHeapPage, firstFittingBlock, numberOfBlocksToBeAllocated);

  // increaseMemoryTracker(numberOfBlocksToBeAllocated * HEAP_BLOCK_SIZE);
  return &freeHeapPage->alloc_memory[firstFittingBlock * HEAP_BLOCK_SIZE];

}