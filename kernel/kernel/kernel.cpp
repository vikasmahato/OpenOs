#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <kernel/basesystem.h>
#include <asm.h>
#include <kernel/multiboot.h>
#include <kernel/tty.h>

#ifdef __cplusplus
extern "C"
{
#endif

void kernel_early() {
    //terminal_initialize();


}

void kernel_main(uint32_t magic, struct multiboot_info* mb) {

    BaseSystem baseSystem;
    baseSystem.init();

    printf("Magic number: %lx\n", magic);
    multiboot_memory_map_t* mm = (multiboot_memory_map_t*) mb->mmap_addr;
    while ((unsigned int) mm < mb->mmap_addr + mb->mmap_length) {
        if (mm->type != MULTIBOOT_MEMORY_AVAILABLE) {
            printf("Unavailable physical address: %llx, type: %u, length: %llx\n",
                   mm->addr, mm->type, mm->len);
        } else {
            printf("Available physical address: %llx, length: %llx\n",
                   mm->addr, mm->len);
        }
        mm = (multiboot_memory_map_t*) ((unsigned int) mm +
                                        mm->size + sizeof(mm->size));
    }

  printf("Hello, kernel World!\n");

 for(;;) {
    asm("hlt");
 }
}

#ifdef __cplusplus
}
#endif