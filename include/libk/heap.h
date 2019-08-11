#ifndef _LIBK_KHEAP_H_
#define _LIBK_KHEAP_H_

/*
 * * References: http://www.jamesmolloy.co.uk/tutorial_html/7.-The%20Heap.html
 * 
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <libk/memlayout.h>
#include <libk/virt_mem.h>

#define HEAP_MAGIC 0xDEADBEEF
#define HEAP_PAGE_ACTUAL_SIZE sizeof(HeapPage)

class HeapPage {
    private:
        size_t magic;                                              /* Magic number, used for error checking and identification. */
        size_t num_blocks_available;                               /* Number of available heap blocks. */
        unsigned char allocated_block_bitmap[HEAP_BLOCK_BIT_MAP_SIZE];  /* Bitmap: 1 represents an alloc block, 0 a free block */
        unsigned char first_allocated_bitmap[HEAP_BLOCK_BIT_MAP_SIZE];  /* Bitmap: 1 represents the starting block of an allocation, else 0 */
        HeapPage* next;                                            /* Next heap page in the heap page list */
    public:
        HeapPage();
        ~HeapPage();
        void setNext(HeapPage* heapPage) { next = heapPage; }
        HeapPage* getNext() { return next; }
        size_t getNumBlocks() { return num_blocks_available; }
        void setNumBlocksAvailable(size_t numBlocks) { num_blocks_available = numBlocks; }
        unsigned char getAllocatedBlockBitmap(size_t index) { return allocated_block_bitmap[index]; }
        unsigned char* getAllocatedBlockBitmap() { return allocated_block_bitmap; }
        unsigned char* getFirstAllocatedBitmap() { return first_allocated_bitmap; }
       // TODO: make alloc_memory private.
       unsigned char alloc_memory[HEAP_BLOCK_COUNT * HEAP_BLOCK_SIZE]; /* Actual memory being referenced by the bitmaps */
        
};

class HeapPageList {
    private:
        size_t magic;   /* Magic number, used for error checking and identification. */
        HeapPage* head; /* Pointer to head of Heap Page Linked List. */
    public:
        HeapPageList() { head = nullptr; }
        HeapPage* getHead() { return head; }
        void setHead(HeapPage* head) { this->head = head; }
        void pushFront(HeapPage* newHead);
};

class Heap {
    private:
        virtual_addr current_heap_address;
        HeapPageList* heapPageList;
        VirtualMemoryManager* virtualMemoryManager;
    public:
        Heap(VirtualMemoryManager* virtualMemoryManager);
        HeapPage* getHeapPageMetaData(void* ptr);
        /* Requests 4KB from the virtual memory to be owned by the heap */
        void requestMemory();
        /* Given a Heap Page and the number of blocks_to_alloc, return the Heap
         * Block number of the first of a sequence of free blocks that can
         * fit blocks_to_alloc. Returns -1 if no sequence exists.
         */
        int32_t findFittingBlockStart(HeapPage* heapPage, size_t blocksToAlloc);
        /* 
         * Returns the first existing heap page in the heap_page_list that can fit the given number of bytes. 
         * If none can be found, returns nullptr
         */
        HeapPage* getFittingHeapPage(size_t blocksToAlloc);
        void allocateBlocks(HeapPage* heapPage, int32_t firstFillingBlock, size_t numberOfBlocks);
};

// TODO: move to separate header
void* kmalloc(size_t bytes); // !Currently it works only for 4000 bytes
void* kcalloc(size_t bytes);
void kfree(void* ptr);

#ifdef __cplusplus
}
#endif

#endif _LIBK_KHEAP_H_