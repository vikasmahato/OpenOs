#ifndef _KERNEL_PAGETABLE_H_
#define _KERNEL_PAGETABLE_H_

#include <stdint.h>

/* Useful macros used for minimal abstraction */
#define PAGES_PER_TABLE        1024
#define PAGE_TABLES_PER_DIR    1024
#define PTABLE_ADDR_SPACE_SIZE 0x400000
#define DTABLE_ADDR_SPACE_SIZE 0x100000000

#ifdef __cplusplus
extern "C"
{
#endif

/* Typedefs for a PTE and a PDE. They're just 32-bit values */
typedef uint32_t page_table_entry;
typedef uint32_t page_directory_entry;

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