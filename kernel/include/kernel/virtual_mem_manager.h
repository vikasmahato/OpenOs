#ifndef _KERNEL_VIR_MEM_MANAGER_H_
#define _KERNEL_VIR_MEM_MANAGER_H_

#include "constants.h"
#include "multiboot.h"
#include "pagetable.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/* These two macros allow us to get the indexes we need to access
 * the right PDE and PTE. */
#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)

/* Gets a physical address from a PTE, leaving room for us to add the offset */
#define PAGE_GET_PHYSICAL_ADDRESS(x) ((x) & 0xFFFFF000)

/* Get the address of a PT from a PDE. Note how all addresses have the lower
 * 12 bits zeroed out -- that's the effect of being page/4kb-aligned! */
#define PAGETABLE_GET_ADDRESS(x) ((x) & 0xFFFFF000)

#ifdef __cplusplus
extern "C"
{
#endif

class VirtualMemoryManager {
    private:
        /* Define bitmasks to check the Present, Write, User, Write-Through, Not-Cacheable,
        accessed, dirty bits, as well as the reserved, available (for use) and frame
        parts of the Page Table Entry (PTE). */
        uint32_t PTE_PAGE_PRESENT       =    1;       /* 00000000000000000000000000000001 */
        uint32_t PTE_PAGE_WRITE         =    2;       /* 00000000000000000000000000000010 */
        uint32_t PTE_PAGE_USER          =    4;       /* 00000000000000000000000000000100 */
        uint32_t PTE_PAGE_WRITETHROUGH  =    8;       /* 00000000000000000000000000001000 */
        uint32_t PTE_PAGE_NOT_CACHEABLE = 0x10;       /* 00000000000000000000000000010000 */
        uint32_t PTE_PAGE_ACCESSED      = 0x20;       /* 00000000000000000000000000100000 */
        uint32_t PTE_PAGE_DIRTY         = 0x40;       /* 00000000000000000000000001000000 */
        uint32_t PTE_PAGE_RESERVED      = 0x180;      /* 00000000000000000000000110000000 */
        uint32_t PTE_PAGE_AVAIL         = 0xE00;      /* 00000000000000000000111000000000 */
        uint32_t PTE_PAGE_FRAME         = 0xFFFFF000; /* 11111111111111111111000000000000 */

        /* Define bitmasks to check the Present, Write, User, Write-Through, Not-Cacheable,
        accessed, dirty bits, as well as the reserved, available (for use) and frame
        parts of the Page Table Entry (PTE). */
        uint32_t PDE_PAGE_PRESENT       =    1;       /* 00000000000000000000000000000001 */
        uint32_t PDE_PAGE_WRITE         =    2;       /* 00000000000000000000000000000010 */
        uint32_t PDE_PAGE_USER          =    4;       /* 00000000000000000000000000000100 */
        uint32_t PDE_PAGE_WRITETHROUGH  =    8;       /* 00000000000000000000000000001000 */
        uint32_t PDE_PAGE_NOT_CACHEABLE = 0x10;       /* 00000000000000000000000000010000 */
        uint32_t PDE_PAGE_ACCESSED      = 0x20;       /* 00000000000000000000000000100000 */
        uint32_t PDE_PAGE_DIRTY         = 0x40;       /* 00000000000000000000000001000000 */
        uint32_t PDE_PAGE_4MB           = 0x180;      /* 00000000000000000000000110000000 */
        uint32_t PDE_PAGE_AVAIL         = 0xE00;      /* 00000000000000000000111000000000 */
        uint32_t PDE_PAGE_FRAME         = 0xFFFFF000; /* 11111111111111111111000000000000 */

        uint32_t PAGE_SIZE  = 0x1000; /* 4kb */
        uint32_t BLOCK_SIZE = PAGE_SIZE;
        uint32_t PAGE_MASK  = 0xFFFFF000; /* Mask constant to page-align an address. */
        uint32_t BLOCK_MASK = PAGE_MASK;

        /* Add an attribute to this PTE. You can choose one of the PTE_PAGE_* macros
        * above as the attrib argument. For instance, to mark a PTE as present and
        * write-enabled.
        * page_table_entry_add_attrib ( &e, PTE_PAGE_PRESENT );
        * page_table_entry_add_attrib ( &e, PTE_PAGE_WRITE );
        * 
        * Assuming that e is the 32-bit Page Entry.
        */ 
        void page_table_entry_add_attrib ( page_table_entry* e, uint32_t attrib );

        /* This deletes an attribute added with page_table_entry_add_attrib. */
        void page_table_entry_del_attrib ( page_table_entry* e, uint32_t attrib );

        /* This assigns a frame to a PTE. That is, it makes this PTE concern that
        * 4kb group of physical memory. */
        void page_table_entry_set_frame ( page_table_entry* e, uint32_t frameaddress );

        /* Functions to check various attributes of a PTE */
        bool page_table_entry_is_present ( page_table_entry e );
        bool page_table_entry_is_writable ( page_table_entry e );

        /* These functions are exactly the same as the page_table_entry_* family of
        * functions, except they concern PDs. */
        void page_directory_entry_add_attrib ( page_directory_entry* e, uint32_t attrib );
        void page_directory_entry_del_attrib ( page_directory_entry* e, uint32_t attrib );
        void page_directory_entry_set_pte_address ( page_directory_entry*, uint32_t pte_address );
        bool page_directory_entry_is_present ( page_directory_entry e );
        bool page_directory_entry_is_writable ( page_directory_entry e );

        /* This function returns a pointer to the PDE used to map the virtual address
        * 'addr' to a physical address. All it does is use PAGE_DIRECTORY_INDEX
        * to find the index into the supplied PD, and then return a pointer to it. */
        page_directory_entry* vmm_page_directory_lookup_entry ( page_directory* p, uint32_t addr );

        /* This function return a pointer to the PTE used to map the virtual address
        * 'addr' to a physical address. All it does is use the PAGE_TABLE_INDEX to find
        * an index into the supplied PT, and then return a pointer to it. Notice how
        * usually one must first use vmm_page_directory_lookup_entry with the same addr
        * to find the required PTE */
        page_table_entry* vmm_page_table_lookup_entry ( page_table* p, uint32_t addr );

        /* Switch the page directory to the supplied PD. The VMM will keep track of
        * this address, and you'll be able to find it with vmm_get_current_directory()
        */
        void vmm_switch_page_directory (page_directory* pd);

        /* Get a pointer to the current PD. As the API developer we do not guarantee
        * that this address is the real physical address of the PD, merely that
        * it is mapped to it. (Though most probably it is identity-mapped) */
        page_directory* vmm_get_current_directory ( void );

        /* Enable and disable paging. NOTE that currently we already boot (see start.s)
        * with paging enabled! */
        void vmm_enable_paging ();
        void vmm_disable_paging ();

        /* Flush this TLB entry. We use this whenever we change a PTE or PDE (FIXME: needed for PDEs? */
        void vmm_flush_tlb_entry ( uint32_t addr );

        /* This maps a virtual page to a physical frame. Note that both virt and phys
        * have to be page/4kb-aligned. The function makes sure that the current PD
        * has the necessary PDEs, creating any necessary PTs and PTEs. It's a somewwhat
        * complex process which can be transparently done with it. */
        void vmm_map_page ( uint32_t phys, uint32_t virt );

    public:
        /* Allocate a random physical page to this PTE. The VMM asks the PMM for
        * a free page (located anywhere in memory) and assigns it to the PTE,
        * marking ir PRESENT and WRITE-able. It's useful if we want to map
        * just about anypage to a specific address, which is what we'll do
        * most of the time. */
        bool vmm_alloc_page (page_table_entry* e);

        /* This undoes what vmm_alloc_page does, returning the page to the PMM
        * and marking the PTE as NOT PRESENT */
        void vmmn_free_page ( page_table_entry* e );

        /* The all mighty function to initialize the VMM. See vmm.c to know
        * what the VMM does when it starts up */
        VirtualMemoryManager();
};

#ifdef __cplusplus
}
#endif

#endif //_KERNEL_VIR_MEM_MANAGER_H_
