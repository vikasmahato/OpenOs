#include <kernel/phy_mem_manager.h>
#include <kernel/virtual_mem_manager.h>
#include <string.h>

bool PhysicalMemoryManager::pmm_paging_active_ = false;

PhysicalMemoryManager::PhysicalMemoryManager(multiboot_info* mb, PhysicalAddress start) {
    if((start & BLOCK_MASK) == start) {
        pmmStartLocation = pmmCurrentLocation = start;
    } else {
        pmmStartLocation = pmmCurrentLocation = (start + BLOCK_SIZE) & BLOCK_MASK;
    }

    printf("Physical Memory Manager installed at %lx\n", pmmStartLocation);
}


PhysicalAddress PhysicalMemoryManager::allocateFrame() {
    if( pmm_paging_active_ ) {
        PhysicalAddress* stack;
        if(pmmStackLocation == PMM_STACK_ADDR){
            // kpanic
            printf("Out of memory \n");
        }
        pmmStackLocation -= sizeof(PhysicalAddress);
        stack = (PhysicalAddress*) pmmStackLocation;
        return *stack;

    } else {
        printf("1\n");
        pmmCurrentLocation += BLOCK_SIZE;
        printf("2\n");
        return pmmCurrentLocation;
    }
}

PhysicalAddress PhysicalMemoryManager::allocateFrames (size_t n) {
    if(pmm_paging_active_) {
        //kpanic
        printf("Cannot allocate large chunks when paging is active  \n");
    }
    return pmmCurrentLocation += (n * BLOCK_SIZE);
}

void PhysicalMemoryManager::freeFrame(PhysicalAddress p) {
    // we cannot free anything below this as its used by vmm
    if( p < pmmCurrentLocation) {
        return;
    }

    // If stack is at its limit we use new page to enlarge the stack
    if(pmmStackMax <= pmmStackLocation) {
        //VirtualMemoryManaget::mapPage();
    } else {
        PhysicalAddress* stack = (PhysicalAddress*) pmmStackLocation;
        *stack = p;
        pmmStackLocation += sizeof(PhysicalAddress);
    }


}



void PhysicalMemoryManager::notifyPagingEnabled() {
    pmm_paging_active_ = true;
}

void PhysicalMemoryManager::notifyPagingDisabled() {
    pmm_paging_active_ = false;
}

PhysicalAddress PhysicalMemoryManager::getStartLocation() {
    return pmmStartLocation;
}