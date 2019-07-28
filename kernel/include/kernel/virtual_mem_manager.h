#ifndef _KERNEL_VIR_MEM_MANAGER_H_
#define _KERNEL_VIR_MEM_MANAGER_H_

#include "constants.h"
#include "multiboot.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "pagetable.h"
#include <kernel/phy_mem_manager.h>

#ifdef __cplusplus
extern "C"
{
#endif

class VirtualMemoryManager {
    private:
        page_directory* current_directory;
        PhysicalMemoryManager* pmm;
        
        /* Enable and disable paging. */
        void enablePaging ();
        void disablePaging ();

        /* Enable and disable 4mb pages. */
        void enable4MbPages ();
        void disable4MbPages ();

        /* Flush this TLB entry. We use this whenever we change a PTE or PDE (FIXME: needed for PDEs? */
        void flushTlbEntry ( VirtualAddress addr );

        /* This function returns a pointer to the PDE used to map the virtual address
        * 'addr' to a physical address. All it does is use PAGE_DIRECTORY_INDEX
        * to find the index into the supplied PD, and then return a pointer to it. */
        page_directory_entry* pageDirectoryLookupEntry ( page_directory* p, VirtualAddress addr );

        /* This function return a pointer to the PTE used to map the virtual address
        * 'addr' to a physical address. All it does is use the PAGE_TABLE_INDEX to find
        * an index into the supplied PT, and then return a pointer to it. Notice how
        * usually one must first use vmm_page_directory_lookup_entry with the same addr
        * to find the required PTE */
        page_table_entry* pageTableLookupEntry ( page_table* p, VirtualAddress addr );

        /* Switch the page directory to the supplied PD. The VMM will keep track of
        * this address, and you'll be able to find it with getCurrentDirectory()
        */
        void switchPageDirectory (page_directory* pd);

        /* Get a pointer to the current PD. As the API developer we do not guarantee
         * that this address is the real physical address of the PD, merely that
         * it is mapped to it. (Though most probably it is identity-mapped) */
        page_directory* getCurrentDirectory ();

    public:
        VirtualMemoryManager(PhysicalMemoryManager* pmm);
        /* Allocate a random physical page to this PTE. The VMM asks the PMM for
         * a free page (located anywhere in memory) and assigns it to the PTE,
         * marking ir PRESENT and WRITE-able. It's useful if we want to map
         * just about anypage to a specific address, which is what we'll do
         * most of the time. */
        bool allocatePage (page_table_entry* e);

        /* This undoes what allocateFrame does, returning the page to the PMM
         * and marking the PTE as NOT PRESENT */
        void freePage ( page_table_entry* e );

        /* This maps a virtual page to a physical frame. Note that both virt and phys
         * have to be page/4kb-aligned. The function makes sure that the current PD
         * has the necessary PDEs, creating any necessary PTs and PTEs. It's a somewwhat
         * complex process */
        void mapPage ( PhysicalAddress phys, VirtualAddress virt );
};

#ifdef __cplusplus
}
#endif

#endif //_KERNEL_VIR_MEM_MANAGER_H_
