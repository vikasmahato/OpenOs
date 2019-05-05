#ifndef _KERNEL_PHY_MEM_MANAGER_H_
#define _KERNEL_PHY_MEM_MANAGER_H_

#include "constants.h"
#include "multiboot.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

class PhysicalMemoryManager {
    private:
        //! 8 blocks per byte
        const uint32_t BLOCKS_PER_BYTE = 8;
        //! block size (4k)
        const uint32_t BLOCK_SIZE = 4096;
        // Physical memory manager
        // Currently implemented using bit map based allocation

        //! memory map bit array. Each bit represents a memory block
        static uint32_t* phys_memory_map_;

        //! size of physical memory
        static uint32_t phys_mem_size_kb_;

        //! number of blocks currently in use
        static uint32_t used_blocks_;

        //! maximum number of available memory blocks
        static uint32_t total_blocks_;

        void mmap_set (int bit);
        void mmap_unset (int bit);
        bool mmap_test (int bit);
        int find_free_block ();

        //! Functions to initialize the Physical Memory Manager
        void free_available_memory(multiboot_info* mb);

        //! Functions to manage a single block in memory
        void*	alloc_block ();
        void	free_block (void* p);

        //! Functions to manage a large blocks in memory
        void allocate_chunk (uint32_t base_addr, size_t size);
        void deallocate_chunk (uint32_t base_addr, size_t size);

    public:
        PhysicalMemoryManager(multiboot_info* mb);

};

#ifdef __cplusplus
}
#endif

#endif //_KERNEL_PHY_MEM_MANAGER_H_
