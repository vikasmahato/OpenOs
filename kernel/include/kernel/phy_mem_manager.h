#ifndef _KERNEL_PHY_MEM_MANAGER_H_
#define _KERNEL_PHY_MEM_MANAGER_H_

#include "constants.h"
#include "multiboot.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif


class PhysicalMemoryManager {
    private:
        static bool pmm_paging_active_;
        PhysicalAddress pmmCurrentLocation;
        PhysicalAddress pmmStartLocation;
        PhysicalAddress pmmStackLocation = PMM_STACK_ADDR;
        PhysicalAddress pmmStackMax = PMM_STACK_ADDR;
        
    public:
        PhysicalMemoryManager(multiboot_info* mb, PhysicalAddress start);
        PhysicalAddress	allocateFrame ();
        PhysicalAddress allocateFrames (size_t n);
        void freeFrame (PhysicalAddress p);
        PhysicalAddress getStartLocation();
        static void notifyPagingEnabled ();
        static void notifyPagingDisabled();
};

#ifdef __cplusplus
}
#endif

#endif //_KERNEL_PHY_MEM_MANAGER_H_
