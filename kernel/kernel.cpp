#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <libk/basesystem.h>

#ifdef __cplusplus
extern "C"
{
#endif

void kernel_early(struct multiboot_info* mb) {
  BaseSystem baseSystem;
  baseSystem.init(mb);
}

void kernel_main(void) {
  // printf("Hello, kernel World %lu!\n", HEAP_PAGE_ACTUAL_SIZE);
  // int a = 10;
  // printf("aia %lx\n", virt_to_phys((virtual_addr)&a));
  for (;;) {
    asm("hlt");
  }
}


#ifdef __cplusplus
}
#endif