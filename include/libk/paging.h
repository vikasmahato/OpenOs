#ifndef _LIBK_KPAGING_H_
#define _LIBK_KPAGING_H_

#include <libk/memlayout.h>
#include <stdbool.h>
#include <stdint.h>

// A Page Table Entry points to a single page stored somewhere in memory/disk

typedef uint32_t pt_entry;


/* Define bitmasks to check the Present, Write, User, Write-Through, Not-Cacheable,
 * accessed, dirty bits, as well as the reserved, available (for use) and frame
 * parts of the Page Table Entry (PTE).
 */
enum PAGE_PTE_FLAGS {
  I86_PTE_PRESENT        = 1,             /* 00000000000000000000000000000001 */
  I86_PTE_WRITABLE       = 2,             /* 00000000000000000000000000000010 */
  I86_PTE_USER           = 4,             /* 00000000000000000000000000000100 */
  I86_PTE_WRITETHOUGH    = 8,             /* 00000000000000000000000000001000 */
  I86_PTE_NOT_CACHEABLE  = 0x10,          /* 00000000000000000000000000010000 */
  I86_PTE_ACCESSED       = 0x20,          /* 00000000000000000000000000100000 */
  I86_PTE_DIRTY          = 0x40,          /* 00000000000000000000000001000000 */
  I86_PTE_PAT            = 0x80,          /* 00000000000000000000000010000000 */
  I86_PTE_CPU_GLOBAL     = 0x100,         /* 00000000000000000000000100000000 */
  I86_PTE_LV4_GLOBAL     = 0x200,         /* 00000000000000000000001000000000 */
  I86_PTE_FRAME          = 0xFFFFF000     /* 11111111111111111111000000000000 */
};

inline void pt_entry_add_attrib(pt_entry* entry, uint32_t attrib) {
  *entry |= attrib;
}

inline void pt_entry_del_attrib(pt_entry* entry, uint32_t attrib) {
  *entry &= ~attrib;
}

inline void pt_entry_set_frame(pt_entry* entry, physical_addr addr) {
  *entry = (*entry & ~I86_PTE_FRAME) | addr;
}

inline bool pt_entry_is_present(pt_entry entry) {
  return entry & I86_PTE_PRESENT;
}

inline bool pt_entry_is_writable(pt_entry entry) {
  return entry & I86_PTE_WRITABLE;
}

inline physical_addr pt_entry_frame(pt_entry entry) {
  return entry & I86_PTE_FRAME;
}

// A Page Directory Entry points to a diretory with 1024 PT Entries

typedef uint32_t pd_entry;

/* Define bitmasks to check the Present, Write, User, Write-Through, Not-Cacheable,
 * accessed, dirty bits, as well as the reserved, available (for use) and frame
 * parts of the Page Table Entry (PTE).
 */
enum PAGE_PDE_FLAGS {
  I86_PDE_PRESENT            = 1,           /* 00000000000000000000000000000001 */
  I86_PDE_WRITABLE           = 2,           /* 00000000000000000000000000000010 */
  I86_PDE_USER               = 4,           /* 00000000000000000000000000000100 */
  I86_PDE_PWT                = 8,           /* 00000000000000000000000000001000 */
  I86_PDE_PCD                = 0x10,        /* 00000000000000000000000000010000 */
  I86_PDE_ACCESSED           = 0x20,        /* 00000000000000000000000000100000 */
  I86_PDE_DIRTY              = 0x40,        /* 00000000000000000000000001000000 */
  I86_PDE_4MB                = 0x80,        /* 00000000000000000000000010000000 */
  I86_PDE_CPU_GLOBAL         = 0x100,       /* 00000000000000000000000100000000 */
  I86_PDE_LV4_GLOBAL         = 0x200,       /* 00000000000000000000001000000000 */
  I86_PDE_FRAME              = 0xFFFFF000   /* 11111111111111111111000000000000 */
};

inline void pd_entry_add_attrib(pd_entry* entry, uint32_t attrib) {
  *entry |= attrib;
}

inline void pd_entry_del_attrib(pd_entry* entry, uint32_t attrib) {
  *entry &= attrib;
}

inline void pd_entry_set_frame(pd_entry* entry, physical_addr addr) {
  *entry = (*entry & ~I86_PDE_FRAME) | addr;
}

inline bool pd_entry_is_present(pd_entry entry) {
  return entry & I86_PDE_PRESENT;
}

inline bool pd_entry_is_user(pd_entry entry) { return entry & I86_PDE_USER; }

inline bool pd_entry_is_4mb(pd_entry entry) { return entry & I86_PDE_4MB; }

inline bool pd_entry_is_writable(pd_entry entry) {
  return entry & I86_PDE_WRITABLE;
}

inline physical_addr pd_entry_frame(pd_entry entry) {
  return entry & I86_PDE_FRAME;
}

inline void pd_entry_enable_global(pd_entry entry) {
  
}

#endif  // _LIBK_KPAGING_H_