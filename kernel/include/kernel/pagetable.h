#ifndef _KERNEL_PAGETABLE_H_
#define _KERNEL_PAGETABLE_H_

#include <stdint.h>
#include "constants.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Define bitmasks to check the Present, Write, User, Write-Through, Not-Cacheable,
   accessed, dirty bits, as well as the reserved, available (for use) and frame
   parts of the Page Table Entry (PTE). */
#define PTE_PAGE_PRESENT          1       /* 00000000000000000000000000000001 */
#define PTE_PAGE_WRITE            2       /* 00000000000000000000000000000010 */
#define PTE_PAGE_USER             4       /* 00000000000000000000000000000100 */
#define PTE_PAGE_WRITETHROUGH     8       /* 00000000000000000000000000001000 */
#define PTE_PAGE_NOT_CACHEABLE 0x10       /* 00000000000000000000000000010000 */
#define PTE_PAGE_ACCESSED      0x20       /* 00000000000000000000000000100000 */
#define PTE_PAGE_DIRTY         0x40       /* 00000000000000000000000001000000 */
#define PTE_PAGE_RESERVED      0x180      /* 00000000000000000000000110000000 */
#define PTE_PAGE_AVAIL         0xE00      /* 00000000000000000000111000000000 */
#define PTE_PAGE_FRAME         0xFFFFF000 /* 11111111111111111111000000000000 */

/* Define bitmasks to check the Present, Write, User, Write-Through, Not-Cacheable,
   accessed, dirty bits, as well as the reserved, available (for use) and frame
   parts of the Page Table Entry (PTE). */
#define PDE_PAGE_PRESENT          1       /* 00000000000000000000000000000001 */
#define PDE_PAGE_WRITE            2       /* 00000000000000000000000000000010 */
#define PDE_PAGE_USER             4       /* 00000000000000000000000000000100 */
#define PDE_PAGE_WRITETHROUGH     8       /* 00000000000000000000000000001000 */
#define PDE_PAGE_NOT_CACHEABLE 0x10       /* 00000000000000000000000000010000 */
#define PDE_PAGE_ACCESSED      0x20       /* 00000000000000000000000000100000 */
#define PDE_PAGE_DIRTY         0x40       /* 00000000000000000000000001000000 */
#define PDE_PAGE_4MB           0x180      /* 00000000000000000000000110000000 */
#define PDE_PAGE_AVAIL         0xE00      /* 00000000000000000000111000000000 */
#define PDE_PAGE_FRAME         0xFFFFF000 /* 11111111111111111111000000000000 */

/* Typedefs for a PTE and a PDE. They're just 32-bit values */
typedef uint32_t page_table_entry;
typedef uint32_t page_directory_entry;

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

/* Useful macros used for minimal abstraction */
#define PAGES_PER_TABLE        1024
#define PAGE_TABLES_PER_DIR    1024
#define PTABLE_ADDR_SPACE_SIZE 0x400000
#define DTABLE_ADDR_SPACE_SIZE 0x100000000

/* These two macros allow us to get the indexes we need to access
 * the right PDE and PTE. */
#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)

/* Gets a physical address from a PTE, leaving room for us to add the offset */
/* FIXME: If we ever change to 64-bit, using ~0xFFF might be better than 
 * 0xFFFFF000!! */
#define PAGE_GET_PHYSICAL_ADDRESS(x) ((x) & 0xFFFFF000)

/* Get the address of a PT from a PDE. Note how all addresses have the lower
 * 12 bits zeroed out -- that's the effect of being page/4kb-aligned! */
#define PAGETABLE_GET_ADDRESS(x) ((x) & 0xFFFFF000)

/* The PMM has these too. It also further documents them */
#ifndef PAGE_SIZE
#define PAGE_SIZE  0x1000 /* 4kb */
#define BLOCK_SIZE PAGE_SIZE
#define PAGE_MASK  0xFFFFF000 /* Mask constant to page-align an address. */
#define BLOCK_MASK PAGE_MASK
#endif

/* This defines a PT, which is, as we've seen, nothing more than an array
 * of 1024 PTEs. */
typedef struct _page_table {

    page_table_entry entries[PAGES_PER_TABLE];
}  __attribute__((packed)) page_table;

/* This defines a PD, which is, as we've seen, nothing more than an array
 * of 1024 PDEs. */
typedef struct _page_directory {

    page_directory_entry entries[PAGE_TABLES_PER_DIR];
}  __attribute__((packed)) page_directory;


#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_PAGETABLE_H_