#include <kernel/phy_mem_manager.h>
#include <string.h>

uint32_t* PhysicalMemoryManager::phys_memory_map_ = 0;
uint32_t PhysicalMemoryManager::phys_mem_size_kb_ = 0;
uint32_t PhysicalMemoryManager::used_blocks_ = 0;
uint32_t PhysicalMemoryManager::total_blocks_ = 0;
bool PhysicalMemoryManager::pmm_paging_active_ = false;

PhysicalMemoryManager::PhysicalMemoryManager(multiboot_info* mb) {
    phys_mem_size_kb_ = mb->mem_upper + mb->mem_lower;
    total_blocks_ = (phys_mem_size_kb_ * 1024) / BLOCK_SIZE;
    used_blocks_ = total_blocks_;
    phys_memory_map_ = (uint32_t*) KERNEL_END_ADDR;
    memset(phys_memory_map_, 0xFF, total_blocks_ / BLOCKS_PER_BYTE);

    // Frees memory GRUB considers available
    free_available_memory(mb);

    // From the freed memory, we need to allocate the ones used by the Kernel
    pmm_alloc_blocks(KERNEL_START_ADDR, KERNEL_SIZE);

    // We also need to allocate the memory used by the Physical Map itself
    pmm_alloc_blocks(*phys_memory_map_, total_blocks_);
    printf("PhysMem Manager installed. %lxKB, %lx blocks, %lx free blocks.\n",
           phys_mem_size_kb_, total_blocks_, total_blocks_ - used_blocks_);
}


void PhysicalMemoryManager::free_available_memory(multiboot_info* mb) {
    multiboot_memory_map_t* mm = (multiboot_memory_map_t*) mb->mmap_addr;
    while ((unsigned int) mm < mb->mmap_addr + mb->mmap_length) {
        if (mm->type == MULTIBOOT_MEMORY_AVAILABLE) {
            pmm_alloc_blocks(mm->addr, mm->len);
        }
        mm = (multiboot_memory_map_t*) ((unsigned int) mm +
                                        mm->size + sizeof(mm->size));
    }
    mmap_set(0);
}

void* PhysicalMemoryManager::pmm_alloc_block() {
    if (used_blocks_ <= 0) {
        return 0;
    }

    int free_block = find_free_block();
    if (free_block == -1) {
        return 0;
    }

    mmap_set(free_block);
    uint32_t addr = free_block * BLOCK_SIZE;
    used_blocks_++;

    return (void*) addr;
}

void PhysicalMemoryManager::pmm_free_block(void* p) {
    uint32_t addr = (uint32_t) p;
    int block = addr / BLOCK_SIZE;

    mmap_unset(block);
    used_blocks_--;
}

inline void PhysicalMemoryManager::mmap_set (int bit) {
    phys_memory_map_[bit / 32] |= (1 << (bit % 32));
}

inline void PhysicalMemoryManager::mmap_unset (int bit) {
    phys_memory_map_[bit / 32] &= ~(1 << (bit % 32));

}

inline bool PhysicalMemoryManager::mmap_test (int bit) {
    return phys_memory_map_[bit / 32] & (1 << (bit % 32));
}

int PhysicalMemoryManager::find_free_block () {
    for (uint32_t i = 0; i < total_blocks_ / 32; i++) {
        uint32_t block = phys_memory_map_[i];
        if (block != 0xFFFFFFFF) {
            for (uint8_t j = 0; j < 32; j++) {
                if ((1 << j) & block) {
                    return (32 * i) + j;
                }
            }
        }
    }
    return -1;
}

void PhysicalMemoryManager::pmm_alloc_blocks (uint32_t base_addr, size_t size) {
    int cur_block_addr = base_addr / BLOCK_SIZE;
    int num_blocks = size / BLOCK_SIZE;
    while (num_blocks-- >= 0) {
        mmap_set(cur_block_addr++);
        used_blocks_--;
    }
}

void PhysicalMemoryManager::pmm_free_blocks(uint32_t base_addr, size_t size) {
    int cur_block_addr = base_addr / BLOCK_SIZE;
    int num_blocks = size / BLOCK_SIZE;

    while (num_blocks--) {
        mmap_set(cur_block_addr++);
        used_blocks_--;
    }
}

void PhysicalMemoryManager::pmm_notify_paging_enabled() {
    pmm_paging_active_ = true;
}

void PhysicalMemoryManager::pmm_notify_paging_disabled() {
    pmm_paging_active_ = false;
}