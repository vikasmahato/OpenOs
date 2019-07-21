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

        //! Paging Enabled means that the VMM has full control of paging,
        static bool pmm_paging_active_;

        void mmap_set (int bit);
        void mmap_unset (int bit);
        bool mmap_test (int bit);
        int find_free_block ();

        //! Functions to initialize the Physical Memory Manager
        void free_available_memory(multiboot_info* mb);

        //! Functions to manage a single block in memory
        void*	pmm_alloc_block ();
        void	pmm_free_block (void* p);

        //! Functions to manage a large blocks in memory
        void pmm_alloc_blocks (uint32_t base_addr, size_t size);
        void pmm_free_blocks (uint32_t base_addr, size_t size);

        /* Used by the VMM to notify the PMM of paging changes
        * Do note that our kernel already boots with paging enabled
        * (see start.s). What we mean by "paging enabled" here is if the kernel
        * has entered its second paging stage, where the VMM is enabled and a new
        * page directory is effectively created (using the PMM itself).
        *
        * In summary: Paging Enabled means that the VMM has full control of paging,
        *             not simply that paging is enabled, because it always is!
        */
        static void pmm_notify_paging_enabled ();
        static void pmm_notify_paging_disabled ();

    public:
        PhysicalMemoryManager(multiboot_info* mb);

};

#ifdef __cplusplus
}
#endif

#endif //_KERNEL_PHY_MEM_MANAGER_H_
