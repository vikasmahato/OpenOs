#include<libk/heap.h>
#include<stdio.h>

inline static void map_set(unsigned char* bitmap, size_t block) {
  if (block > HEAP_BLOCK_COUNT) {
    return;
  }
  bitmap[block / 8] |= (1 << (block % 8));
}

HeapPage::HeapPage() {
    magic = HEAP_MAGIC;
    num_blocks_available = HEAP_BLOCK_COUNT;
}

Heap::Heap(VirtualMemoryManager* virtualMemoryManager) {
    this->virtualMemoryManager = virtualMemoryManager;
    current_heap_address = HEAP_VIRT_ADDR_START;
    HeapPageList heapPageList;
    this->heapPageList = &heapPageList;
    printf("Kernel heap installed.\n");
}

void HeapPageList::pushFront(HeapPage* newHead) {
    HeapPage* currentHead = head;
    head = newHead;
    newHead->setNext(currentHead);
}

void Heap::requestMemory() {
    HeapPage* newHeapPage = (HeapPage*) current_heap_address;
    if(!virtualMemoryManager->alloc_page(current_heap_address)) {
        /*Could not allocate virtual memory. Aborting.*/
        return;
    }
    heapPageList->pushFront(newHeapPage);
    current_heap_address += PAGE_SIZE;
}

int32_t Heap::findFittingBlockStart(HeapPage* heapPage, size_t blocksToAlloc) {
    size_t startingBlock = 0;
    size_t startingBlockBit = 0;
    size_t curBlockNum = 0;
    
    for (size_t i = 0; i < HEAP_BLOCK_BIT_MAP_SIZE; i++) {
    unsigned char curByte = heapPage->getAllocatedBlockBitmap(i);
    if (curByte == 0xFF) {
      curBlockNum = 0;
      continue;
    }

    for (size_t j = 0; j < 8; j++) {
      int bit = 1 << j;
      if (bit & curByte) {  // bit is set
        curBlockNum = 0;
        continue;
      }

      if (curBlockNum == 0) {
        startingBlock = i;
        startingBlockBit = j;
      }
      curBlockNum += 1;

      if (curBlockNum == blocksToAlloc) {
        // We found a sequence that can fit, return the beginning block number
        return (8 * startingBlock) + startingBlockBit;
      }
    }
  }
  return -1;
}

HeapPage* Heap::getFittingHeapPage(size_t blocksToAlloc) {
    /* If list is empty return nullptr. */
    if(heapPageList->getHead() == nullptr) {
        return nullptr;
    }

    HeapPage* current = heapPageList->getHead();
    
    while(current && current->getNumBlocks() < blocksToAlloc) {
        current = current->getNext();
    }

    return current;
}


void Heap::allocateBlocks(HeapPage* heapPage,
                     int32_t firstFittingBlock,
                     size_t blocksToAlloc) {
  // Mark the beginning of the block in the first_alloced_bitmap
  map_set(heapPage->getFirstAllocatedBitmap(), firstFittingBlock);

  // Mark all the alloced blocks in the alloced_block_bitmap
  for (size_t i = firstFittingBlock; 
       i < firstFittingBlock + blocksToAlloc;
       i++) {
    map_set(heapPage->getAllocatedBlockBitmap(), i);
  }
  // Update the num_available_blocks
  heapPage->setNumBlocksAvailable(heapPage->getNumBlocks() - blocksToAlloc);
}
