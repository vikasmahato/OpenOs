#include<libk/heap_mem.h>
#include<stdio.h>
#include<assert.h>

static uint8_t header_t_less_than(void*a, void *b) {
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
    if ((start_addr & 0xFFFFF000) != 0) {
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

void* HeapMemoryManager::alloc(size_t size, bool page_align) {
    // Make sure we take the size of header/footer into account.
   size_t new_size = size + sizeof(header_t) + sizeof(footer_t);
   // Find the smallest hole that will fit.
   int32_t iterator = findSmallestHole(new_size, page_align);

   if (iterator == -1) { // If we didn't find a suitable hole
     // Save some previous data.
       uint32_t old_length = end_address - start_address;
       uint32_t old_end_address = end_address;

       // We need to allocate some more space.
       expand(old_length+new_size);
       uint32_t new_length = end_address-start_address;

       // Find the endmost header. (Not endmost in size, but in location).
       iterator = 0;
       // Vars to hold the index of, and value of, the endmost header found so far.
       int32_t idx = -1; uint32_t value = 0x0;
       while (iterator < (int32_t)indexTable->getSize())
       {
           uint32_t tmp = (uint32_t) indexTable->findAtIndex(iterator);
           if (tmp > value)
           {
               value = tmp;
               idx = iterator;
           }
           iterator++;
       }

       // If we didn't find ANY headers, we need to add one.
       if (idx == -1)
       {
           header_t *header = (header_t *)old_end_address;
           header->magic = HEAP_MAGIC;
           header->size = new_length - old_length;
           header->is_hole = 1;
           footer_t *footer = (footer_t *) (old_end_address + header->size - sizeof(footer_t));
           footer->magic = HEAP_MAGIC;
           footer->header = header;
           indexTable->insertNode((void*)header);
       }
       else
       {
           // The last header needs adjusting.
           header_t *header = (header_t *) indexTable->findAtIndex(idx); // TODO: doubtful
           header->size += new_length - old_length;
           // Rewrite the footer.
           footer_t *footer = (footer_t *) ( (uint32_t)header + header->size - sizeof(footer_t) );
           footer->header = header;
           footer->magic = HEAP_MAGIC;
       }
       // We now have enough space. Recurse, and call the function again.
       return alloc(size, page_align);
   } 

   header_t *orig_hole_header = (header_t *) indexTable->findAtIndex(iterator);
   uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
   uint32_t orig_hole_size = orig_hole_header->size;
   // Here we work out if we should split the hole we found into two parts.
   // Is the original hole size - requested hole size less than the overhead for adding a new hole?
   if (orig_hole_size - new_size < sizeof(header_t)+sizeof(footer_t))
   {
       // Then just increase the requested size to the size of the hole we found.
       size += orig_hole_size-new_size;
       new_size = orig_hole_size;
   }

   // If we need to page-align the data, do it now and make a new hole in front of our block.
   if (page_align && orig_hole_pos&0xFFFFF000)
   {
       uint32_t new_location   = orig_hole_pos + 0x1000 /* page size */ - (orig_hole_pos&0xFFF) - sizeof(header_t);
       header_t *hole_header = (header_t *)orig_hole_pos;
       hole_header->size     = 0x1000 /* page size */ - (orig_hole_pos&0xFFF) - sizeof(header_t);
       hole_header->magic    = HEAP_MAGIC;
       hole_header->is_hole  = 1;
       footer_t *hole_footer = (footer_t *) ( (int32_t)new_location - sizeof(footer_t) );
       hole_footer->magic    = HEAP_MAGIC;
       hole_footer->header   = hole_header;
       orig_hole_pos         = new_location;
       orig_hole_size        = orig_hole_size - hole_header->size;
   }
   else
   {
       // Else we don't need this hole any more, delete it from the index.
       indexTable->removeAtIndex(iterator);
   }

   // Overwrite the original header...
   header_t *block_header  = (header_t *)orig_hole_pos;
   block_header->magic     = HEAP_MAGIC;
   block_header->is_hole   = 0;
   block_header->size      = new_size;
   // ...And the footer
   footer_t *block_footer  = (footer_t *) (orig_hole_pos + sizeof(header_t) + size);
   block_footer->magic     = HEAP_MAGIC;
   block_footer->header    = block_header;

   // We may need to write a new hole after the allocated block.
   // We do this only if the new hole would have positive size...
   if (orig_hole_size - new_size > 0)
   {
       header_t *hole_header = (header_t *) (orig_hole_pos + sizeof(header_t) + size + sizeof(footer_t));
       hole_header->magic    = HEAP_MAGIC;
       hole_header->is_hole  = 1;
       hole_header->size     = orig_hole_size - new_size;
       footer_t *hole_footer = (footer_t *) ( (int32_t)hole_header + orig_hole_size - new_size - sizeof(footer_t) );
       if ((uint32_t)hole_footer < end_address)
       {
           hole_footer->magic = HEAP_MAGIC;
           hole_footer->header = hole_header;
       }
       // Put the new hole in the index;
       indexTable->insertNode((void*)hole_header);
   }

   // ...And we're done!
   return (void *) ( (uint32_t)block_header+sizeof(header_t) );
}

void HeapMemoryManager::free(void *p) {
    // Exit gracefully for null pointers.
    if (p == 0)
        return;

    // Get the header and footer associated with this pointer.
   header_t *header = (header_t*) ( (int32_t)p - sizeof(header_t) );
   footer_t *footer = (footer_t*) ( (int32_t)header + header->size - sizeof(footer_t) );

   // Sanity checks.
   assert(header->magic == HEAP_MAGIC);
   assert(footer->magic == HEAP_MAGIC);

   // Make us a hole.
   header->is_hole = 1;

   // Do we want to add this header into the 'free holes' index?
   bool do_add = true;

    // Unify left
   // If the thing immediately to the left of us is a footer...
   footer_t *test_footer = (footer_t*) ( (int32_t)header - sizeof(footer_t) );
   if (test_footer->magic == HEAP_MAGIC &&
       test_footer->header->is_hole == 1)
   {
       int32_t cache_size = header->size; // Cache our current size.
       header = test_footer->header;     // Rewrite our header with the new one.
       footer->header = header;          // Rewrite our footer to point to the new header.
       header->size += cache_size;       // Change the size.
       do_add = false;                   // Since this header is already in the index, we don't want to add it again.
   }

   // Unify right
   // If the thing immediately to the right of us is a header...
   header_t *test_header = (header_t*) ( (int32_t)footer + sizeof(footer_t) );
   if (test_header->magic == HEAP_MAGIC &&
       test_header->is_hole)
   {
       header->size += test_header->size; // Increase our size.
       test_footer = (footer_t*) ( (int32_t)test_header + // Rewrite it's footer to point to our header.
                                   test_header->size - sizeof(footer_t) );
       footer = test_footer;
       // Find and remove this header from the index.
       int32_t iterator = 0;
       while ( (iterator < (int32_t)indexTable->getSize()) &&
               (indexTable->findAtIndex(iterator) != (void*)test_header) )
           iterator++;

       // Make sure we actually found the item.
       assert(iterator < (int32_t)indexTable->getSize());
       // Remove it.
       indexTable->removeAtIndex(iterator);
   }

   // If the footer location is the end address, we can contract.
   if ( (int32_t)footer+sizeof(footer_t) == end_address)
   {
       int32_t old_length = end_address - start_address;
       int32_t new_length = contract( (int32_t)header - start_address);
       // Check how big we will be after resizing.
       if (header->size - (old_length-new_length) > 0)
       {/* TODO
           // We will still exist, so resize us.
           header->size -= old_length - new_length;
           footer = (footer_t*) ( (int32_t)header + header->size - sizeof(footer_t) );
           footer->magic = HEAP_MAGIC;
           footer->header = header;*/
       }
       else
       {
           // We will no longer exist :(. Remove us from the index.
           int32_t iterator = 0;
           while ( (iterator < (int32_t)indexTable->getSize()) &&
                   (indexTable->findAtIndex(iterator) != (void*)test_header) )
               iterator++;
           // If we didn't find ourselves, we have nothing to remove.
           if (iterator < (int32_t)indexTable->getSize())
               indexTable->removeAtIndex(iterator);
       }
   }

    if (do_add)
        indexTable->insertNode((void*) header); 

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
           if (((location+sizeof(header_t)) & 0xFFFFF000) != 0)
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

void HeapMemoryManager::expand(size_t new_size) {
    // Sanity check.
   assert(new_size > end_address - start_address);
   // Get the nearest following page boundary.
   if ((new_size&0xFFFFF000) != 0)
   {
       new_size &= 0xFFFFF000;
       new_size += PAGE_SIZE_HEX;
   }
   // Make sure we are not overreaching ourselves.
   assert(start_address+new_size <= max_address);

   // This should always be on a page boundary.
   uint32_t old_size = end_address - start_address;
   uint32_t i = old_size;
   while (i < new_size)
   {
       virtualMemoryManager->alloc_page(i);
       i += PAGE_SIZE_HEX /* page size */;
   }
   end_address = start_address+new_size;
}

size_t HeapMemoryManager::contract(size_t new_size) {
    // TODO
    return new_size;
}