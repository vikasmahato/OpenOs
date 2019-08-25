#include<libk/heap_mem.h>
#include<stdio.h>

static uint8_t header_t_less_than(void*a, void *b)
{
   return (((header_t*)a)->size < ((header_t*)b)->size)?1:0;
}

HeapMemoryManager::HeapMemoryManager(VirtualMemoryManager *virtualMemoryManager, uint32_t start_addr, uint32_t end_address, 
                            uint32_t max_address, bool supervisor, bool readonly) {
    this->virtualMemoryManager = virtualMemoryManager;
    this->end_address = end_address;
    this->max_address = max_address;
    this->supervisor = supervisor;
    this->readonly = readonly;

    for (virtual_addr vaddr = HEAP_VIRT_ADDR_START; vaddr < HEAP_VIRT_ADDR_START+HEAP_INITIAL_BLOCK_SIZE; vaddr += PAGE_SIZE_HEX) {
        virtualMemoryManager->alloc_page(vaddr);
    }

    // Initialise the index.
    OrderedArray orderedArray = OrderedArray((virtual_addr*)start_addr, HEAP_INDEX_SIZE, &header_t_less_than);
    this->indexTable = &orderedArray;

    // Shift the start address forward to resemble where we can start putting data.
    start_addr += sizeof(type_t)*HEAP_INDEX_SIZE;
    // Make sure the start address is page-aligned.
    if (start_addr & 0xFFFFF000 != 0) {
        start_addr &= 0xFFFFF000;
        start_addr += 0x1000;
    }
    this->start_address = start_addr;

    // We start off with one large hole in the index.
   header_t *hole = (header_t *)start_addr;
   hole->size = (size_t) end_address-start_addr;
   hole->magic = HEAP_MAGIC;
   hole->is_hole = true;
   this->indexTable->insertNode((void*)hole);

   printf("Heap Memory initialized at %lx.\n", start_addr);

}

void* HeapMemoryManager::alloc(uint32_t size, bool page_align) {
    return nullptr;
}

void HeapMemoryManager::free(void *p) {

}

int32_t HeapMemoryManager::findSmallestHole(size_t size, bool page_align) {
    // Find the smallest hole that will fit.
   size_t iterator = 0;
   while (iterator < indexTable->getSize())
   {
       header_t *header = (header_t *) indexTable->findAtIndex(iterator);
       // If the user has requested the memory be page-aligned
       if (page_align)
       {
           // Page-align the starting point of this header.
           uint32_t location = (uint32_t)header;
           int32_t offset = 0;
           if ((location+sizeof(header_t)) & (0xFFFFF000 != 0x0))
               offset = PAGE_SIZE_HEX  - (location+sizeof(header_t))%PAGE_SIZE_HEX;
           uint32_t hole_size = (uint32_t)header->size - offset;
           // Can we fit now?
           if (hole_size >= (uint32_t)size)
               break;
       }
       else if (header->size >= size)
           break;
       iterator++;
   }
   // Why did the loop exit?
   if (iterator == indexTable->getSize())
       return -1; // We got to the end and didn't find anything.
   else
       return iterator;
}