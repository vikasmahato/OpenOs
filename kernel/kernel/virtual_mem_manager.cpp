#include <kernel/virtual_mem_manager.h>
#include <kernel/phy_mem_manager.h>
#include <string.h>

/* Fetches any page and adds it to this page table entry (thus mapping it) */
bool VirtualMemoryManager::allocatePage ( page_table_entry* e )
{
    PhysicalAddress p = pmm->allocateFrame();
    if ( p == 0 )
        return false;

    page_table_entry_set_frame ( e, p );
    page_table_entry_add_attrib ( e, PTE_PAGE_PRESENT );
    page_table_entry_add_attrib ( e, PTE_PAGE_WRITE );

    return true;
}

void VirtualMemoryManager::freePage ( page_table_entry* e )
{

    PhysicalAddress p = PAGE_GET_PHYSICAL_ADDRESS ( *e );
    if ( p != 0 )
        pmm->freeFrame ( p );

    page_table_entry_del_attrib ( e, PTE_PAGE_PRESENT );
    page_table_entry_del_attrib ( e, PTE_PAGE_WRITE );
}

page_table_entry* VirtualMemoryManager::pageTableLookupEntry ( page_table* p, VirtualAddress addr )
{

    if ( p )
        return &p->entries[ PAGE_TABLE_INDEX ( addr ) ];
    return 0;
}

page_directory_entry* VirtualMemoryManager::pageDirectoryLookupEntry ( page_directory* p, VirtualAddress addr )
{

    if ( p )
        return &p->entries[ PAGE_DIRECTORY_INDEX ( addr ) ];
    return 0;
}

void VirtualMemoryManager::switchPageDirectory ( page_directory* pd )
{
    current_directory = pd;
    __asm volatile ( "mov %0, %%cr3" : : "r" ( pd ) );
}

void VirtualMemoryManager::enablePaging ()
{
    uint32_t cr0;
    __asm volatile ( "mov %%cr0, %0" : "=r" ( cr0 ) );
    cr0 |= 0x80000000;
    __asm volatile ( "mov %0, %%cr0" : : "r" ( cr0 ) );
}

void VirtualMemoryManager::disablePaging ()
{
    uint32_t cr0;
    __asm volatile ( "mov %%cr0, %0" : "=r" ( cr0 ) );
    cr0 &= ~0x80000000;
    __asm volatile ( "mov %0, %%cr0" : : "r" ( cr0 ) );
}

void VirtualMemoryManager::enable4MbPages ()
{
    uint32_t cr4;
    __asm volatile ( "mov %%cr4, %0" : "=r" ( cr4 ) );
    cr4 |= 0x00000010;
    __asm volatile ( "mov %0, %%cr4" : : "r" ( cr4 ) );
}

void VirtualMemoryManager::disable4MbPages ()
{
    uint32_t cr4;
    __asm volatile ( "mov %%cr4, %0" : "=r" ( cr4 ) );
    cr4 &= ~0x00000010;
    __asm volatile ( "mov %0, %%cr4" : : "r" ( cr4 ) );
}

page_directory* VirtualMemoryManager::getCurrentDirectory ()
{
    return current_directory;
}

void VirtualMemoryManager::flushTlbEntry ( VirtualAddress addr )
{
    /* FIXME: Do a cli and then a sti here? */
    __asm volatile ( "invlpg (%0)" : : "a" ( addr ) );
}

void VirtualMemoryManager::mapPage ( PhysicalAddress phys, VirtualAddress virt )
{
    page_table* table;
    page_table_entry* page;
    printf("Phy: %lx  Virt: %lx\n", phys, virt);
    /* Get the current page directory */
    page_directory* pageDirectory = getCurrentDirectory ();
    
    /* Get the page directory entry for this virtual address (which points to
     * its page table*/
    page_directory_entry* e = pageDirectoryLookupEntry ( pageDirectory, virt );;

    /* Check we have a page table. If we don't, create it */
    if ( !page_directory_entry_is_present ( *e ) ) {
        /* Allocate a new 4kb block where we'll store the page table */
        table = ( page_table* ) pmm->allocateFrame();
        printf("Table: %lx\n", table);
        memset ( table, 0, sizeof ( page_table ) );
        printf("memset done\n");
        /* Mark the page table entry present, writable, and make it point to
         * the table we've just allocated */
        page_directory_entry_add_attrib ( e, PDE_PAGE_PRESENT );
        page_directory_entry_add_attrib ( e, PDE_PAGE_WRITE );
        page_directory_entry_set_pte_address ( e, ( uint32_t ) table );
    } else
        table = ( page_table* ) PAGETABLE_GET_ADDRESS ( *e );

    /* table points to the address' page table */

    /* Find the page table entry where we need to map this page,
     * and map it present and writeable */
    page = pageTableLookupEntry ( table, virt );
    page_table_entry_add_attrib ( page, PTE_PAGE_PRESENT );
    page_table_entry_add_attrib ( page, PTE_PAGE_WRITE );
    
    /* The address at virt (remember it's 4kb aligned!) points to phys, as well as
     * its whole 4kb range */
    page_directory_entry_set_pte_address ( page, phys );
}

/* We're going to enable paging, and we're not going to use 4MB pages.
 * We need to create a new page directory and map 0xC0000000 to
 * 0x0, as we did in the boot phase (it's the higher-half kernel). Notice
 * that in start.s we did this using only one page directory entry, because
 * we used 4MB pages. Since we're now using 4kb pages, we need several entries
 * to map the whole 4MB range we had mapped before (we also didn't need to map
 * the whole 4MB, but we choose to).
 * 
 * After mapping 0xC0000000 to 0x0, we need to identity-map our PMM's working area.
 * start.s already did this, which enabled the PMM to work. We need to map them too.
 * Note that currently, the PMM lives at 0xB0000000 and grows up.
 * 
 * Once that's done, we switch page directory to our new page directory, disable
 * 4 MB pages and notify the PMM that 4kb-paging has been enabled.
 */
VirtualMemoryManager::VirtualMemoryManager(PhysicalMemoryManager* pmm1)
{
    pmm = pmm1;

    uint32_t frame, virt;
 
    current_directory = ( page_directory* ) pmm->allocateFrame();
 
    memset ( current_directory, 0, sizeof ( page_directory ) );
   
    /* Map 0xC0000000 to 0x0 for a 4MB range */
    for ( frame = 0, virt = 0xC0000000 ; frame < 0x400000; virt += 4096, frame += 4096 )
        mapPage ( frame, virt );

    /* Identity map the PMM for 4MB */
    for ( frame = pmm->getStartLocation(), virt = pmm->getStartLocation() ; virt < pmm->getStartLocation() + 0x400000; virt += 4096, frame += 4096 )
        mapPage ( frame, virt );

    switchPageDirectory ( current_directory );
    disable4MbPages();
    enablePaging();
    pmm->notifyPagingEnabled();
}
