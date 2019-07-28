#include <kernel/pagetable.h>

void page_table_entry_add_attrib ( page_table_entry* e, uint32_t attrib )
{
    *e |= attrib;
}

void page_table_entry_del_attrib ( page_table_entry* e, uint32_t attrib )
{
    *e &= ~attrib;
}

/* Note how we make sure we clear e (in case it had another address there)
 * and f (in case it wasn't page/4kb-aligned. */
void page_table_entry_set_frame ( page_table_entry* e, uint32_t f )
{
    *e &= ~PTE_PAGE_FRAME;
    *e |= ( f & PTE_PAGE_FRAME );
}

bool page_table_entry_is_present ( page_table_entry e )
{
    return ( e & PTE_PAGE_PRESENT ) == PTE_PAGE_PRESENT;
}
bool page_table_entry_is_writable ( page_table_entry e )
{
    return ( e & PTE_PAGE_WRITE ) == PTE_PAGE_WRITE;
}

void page_directory_entry_add_attrib ( page_directory_entry* e, uint32_t attrib )
{
    *e |= attrib;
}
void page_directory_entry_del_attrib ( page_directory_entry* e, uint32_t attrib )
{
    *e &= ~attrib;
}

/* Note how we make sure we clear e (in case it had another address there)
 * and f (in case it wasn't page/4kb-aligned. */
void page_directory_entry_set_pte_address ( page_directory_entry* e, uint32_t f )
{
    *e &= ~PDE_PAGE_FRAME;
    *e |= ( f & PDE_PAGE_FRAME );
}
bool page_directory_entry_is_present ( page_directory_entry e )
{
    return ( e & PDE_PAGE_PRESENT ) == PDE_PAGE_PRESENT;
}
bool page_directory_entry_is_writable ( page_directory_entry e )
{
    return ( e & PDE_PAGE_WRITE ) == PDE_PAGE_WRITE;
}