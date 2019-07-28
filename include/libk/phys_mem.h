#ifndef _LIBK_KPHYS_MEM_H_
#define _LIBK_KPHYS_MEM_H_

#include <libk/memlayout.h>
#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif



class PhysicalMemoryManager {
    private:
    static uint32_t* phys_memory_map_;
    static uint32_t phys_mem_size_kb_;
    static uint32_t used_blocks_;
    static uint32_t total_blocks_;

        // Functions to manipulate the bitmap
        static void map_set(int bit) {
            phys_memory_map_[bit / 32] |= (1 << (bit % 32));
        }

        static void map_unset(int bit) {
          phys_memory_map_[bit / 32] &= ~(1 << (bit % 32));
        }

        static bool map_test(int bit) {
            return phys_memory_map_[bit / 32] & (1 << (bit % 32));
        }

        int find_free_block();
        int find_free_blocks(uint32_t count);
        void allocate_chunk(int base_addr, int length);
        void free_chunk(int base_addr, int length);
        void free_available_memory(struct multiboot_info* mb);
    public:
        static uint32_t kernel_phys_map_start;
        static uint32_t kernel_phys_map_end;
    
        PhysicalMemoryManager(multiboot_info* mb);

        void update_map_addr(physical_addr);

        physical_addr alloc_block();
        physical_addr alloc_blocks(uint32_t count);

        void free_block(physical_addr);
        void free_blocks(physical_addr, uint32_t count);

        bool is_alloced(physical_addr);
};

#ifdef __cplusplus
}
#endif

#endif  // _LIBK_KPHYS_MEM_H_