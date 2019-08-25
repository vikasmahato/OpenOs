#ifndef _LIBK_KHEAP_MEM_H_
#define _LIBK_KHEAP_MEM_H_ 1

#include <data_structures/ordered_array.h>
#include <libk/memlayout.h>
#include <libk/virt_mem.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * A heap is a vital component of both application programs and the kernel.
 * It is also generally superseded by a higher level of memory management 
 * that deals with larger chunks of memory. For most operating systems memory 
 * will be allocated based on pages or other large chunks. A page on the 
 * X86 and X64 architectures is generally 4KB, but can be larger. However, 
 * for smaller allocations the entire page would be wasted. For example if 
 * you only needed 24 bytes and you allocate an entire 4KB page you will 
 * have wasted a lot of memory. So most applications and kernels will 
 * implemented a second memory management scheme that uses memory allocated 
 * in 4KB chunks (or larger) and break these strips of pages or individual 
 * pages into smaller parts as they are requested.
 * 
 * The algorithm we use for our heap is inspired by James Molly's tutorials
 * due to its simple design. It uses two concepts: blocks and holes. 
 * Blocks are contiguous areas of memory containing user data currently in use 
 * (i.e. malloc()d but not free()d). Holes are blocks but their contents are 
 * not in use. So initially by this concept the entire area of heap space is 
 * one large hole. For every hole there is a corresponding descriptor in an 
 * index table. The index table is always ordered ascending by the size of 
 * the hole pointed to. Blocks and holes each contain descriptive data - 
 * a header and a footer. The header contains the most information about the 
 * block - the footer merely contains a pointer to the header.
 * 
 * *Allocation
 * Allocation is straightforward, if a little long-winded. Most of the steps 
 * are error-checking and creating new holes to minimise memory leaks.
 * - Search the index table to find the smallest hole that will fit the requested 
 *   size. As the table is ordered, this just entails iterating through until we 
 *   find a hole which will fit.
 *     - If we didn't find a hole large enough, then:
 *          - Expand the heap.
 *          - If the index table is empty (no holes have been recorded) then add a new entry to it.
 *          - Else, adjust the last header's size member and rewrite the footer.
 *          - To ease the number of control-flow statements, we can just recurse 
 *            and call the allocation function again, trusting that this time there 
 *            will be a hole large enough.
 * - Decide if the hole should be split into two parts. This will normally be the case - 
 *   we usually will want much less space than is available in the hole. The only time 
 *   this will not happen is if there is less free space after allocating the block than 
 *   the header/footer takes up. In this case we can just increase the block size and 
 *   reclaim it all afterwards.
 * - If the block should be page-aligned, we must alter the block starting address so 
 *   that it is and create a new hole in the new unused area.
 *      - If it is not, we can just delete the hole from the index.
 * - Write the new block's header and footer.
 * - If the hole was to be split into two parts, do it now and write a new hole into the index.
 * - Return the address of the block + sizeof(header_t) to the user.
 *
 * * Deallocation
 * - Find the header by taking the given pointer and subtracting the sizeof(header_t).
 * - Sanity checks. Assert that the header and footer's magic numbers remain in tact.
 * - Set the is_hole flag in our header to 1.
 * - If the thing immediately to our left is a footer:
 *     - Unify left. In this case, at the end of the algorithm we shouldn't add our header to the hole index (the header we are unifying with is already there!) so set a flag which the algorithm checks later.
 * - If the thing immediately to our right is a header:
 *     - Unify right.
 * - If the footer is the last in the heap ( footer_location+sizeof(footer_t) == end_address ):
 *     - Contract.
 * - Insert the header into the hole array unless the flag described in Unify left is set.
 * 
 * References:
 * - https://wiki.osdev.org/Heap
 * - http://www.jamesmolloy.co.uk/tutorial_html/7.-The%20Heap.html
 */

/* Size information for a hole/block */
typedef struct {
   uint32_t magic;      /* Magic number, used for error checking and identification. */
   bool is_hole;        /* 1 if this is a hole. 0 if this is a block. */
   uint32_t size;       /* size of the block, including the end footer. */
} header_t;

typedef struct {
   uint32_t magic;      /* Magic number, same as in header_t. */
   header_t *header;    /* Pointer to the block header. */
} footer_t;

class HeapMemoryManager {
    private:
        VirtualMemoryManager *virtualMemoryManager;
        OrderedArray* indexTable;
        uint32_t start_address; /* The start of our allocated space. */
        uint32_t end_address;   /* The end of our allocated space. May be expanded up to max_address. */
        uint32_t max_address;   /* The maximum address the heap can be expanded to. */
        bool supervisor;        /* Should extra pages requested by us be mapped as supervisor-only? */
        bool readonly;          /* Should extra pages requested by us be mapped as read-only? */

        int32_t findSmallestHole(size_t size, bool page_align);
        void expand(size_t new_size);
        size_t contract(size_t new_size);

    public:
        HeapMemoryManager(VirtualMemoryManager *virtualMemoryManager, uint32_t start, uint32_t end, 
                            uint32_t max, bool supervisor, bool readonly);
        /* 
         * Allocates a contiguous region of memory 'size' in size. 
         * @param page_alaign If true, it creates that block starting on a page boundary. 
         */
        void *alloc(size_t size, bool page_align);
        /* Releases a block allocated with 'alloc'. */
        void free(void *p); 
};

#ifdef __cplusplus
}
#endif

#endif // _LIBK_KHEAP_MEM_H_

