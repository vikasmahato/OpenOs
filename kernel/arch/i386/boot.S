
# start.s -- Kernel entry point and definition of important data to be written
# to the kernel, so that GRUB can find it.
#
# This file is responsible for:
# * Enabling GRUB to find us, by placing the magic number somewhere where grub
#   should start running code.
# * Using only position-independent code, setup a higher-half kernel. That is,
#   a kernel which lives above the 2 GB barrier. Specifically, our higher-half
#   kernel is a 3/1 kernel, residing at 0xC0100000 (technically a 3/1 resides
#   at 0xC0000000, but keep reading for why we chose this address)
# * Map a region of memory (0xB0000000) to itself, so it can later be used by
#   the PMM (Physical Memory Manager) for memory management (also see pmm.h)
# * Setting up a new stack for our kernel
# * Jump to kernel_main and pass it a grub info structure
#
#
# --------------------- HOW THE HIGHER-HALF KERNEL WORKS ---------------------
# Our linker.ld script is configured so that all functions and variables are
# assumed to be from address 0xC0100000 and onwards. This means that the
# kernel really is ready to be run at 0xC0100000. We will discuss why we
# chose this address in the end of this explanation.
#
# However, GRUB can't place/load the kernel there. It loads us in the first
# megabyte of memory (because we tell it to in the linker script).
# We need to find a way to have 0xC0000000 "point" to 0x0, and
# 0xC0100000 point to 0x100000 (1mb). We do this with paging.
#
# Indeed, if we enable paging correctly, we can map the area where the kernel
# expects to be loaded to the area where it really is loaded. And since paging
# will be enabled from there on, there is no problem if it isn't physically
# there.
#
# To accomplish this, the .data section in this file has a Page Directory
# already built with all the page directory entries ready on the spot (built
# with macros during the assembly phase). When GRUB loads us, we only need to
# setup paging with this page directory. And for this, we must use
# position-independent code -- remember that the kernel expects to be loaded
# at 0xC0000000, so it can't rely on that!
#
# This page directory doesn't just map 0xC0000000. If we only mapped it, then
# when we enabled paging, we'd hit a page-fault. Why? Because since the kernel
# is effectively loaded on the lower 1mb addresses, EIP, holding the next
# instruction to be run exactly after enabling paging, will be pointing to
# code in this 1mb area, which would be unmapped! This means that we must
# also map 1mb to itself, so that the value of EIP before we enable paging
# is still valid after we enable it. The page directory, then, has two
# distinct entries to do the mapping -- one for indentity-mapping, and the
# other for mapping the kernel to the higher-half (rather, mapping the
# higher-half to the lower-addresses).
#
# Once paging has been enabled, we can jump to functions which are at
# 0xC0100000 (note that physically they are still in the 1mb area, but
# paging handles that for us and makes it really look like they're at
# 0xC0100000). Since now the kernel is where it is supposed to be
# (even if only from a virtual perspective), everything can work out of the
# box, and we no longer need position-independent code. Thus, after the jump
# to a function in the 0xC0100000 area, EIP will be holding a value valid
# in this range, and we can undo the indentity-mapping of the 1mb area,
# keeping only the mapping of our kernel.
#
# (Why 0xC0100000? Why not 0xC0000000? Let's start by remembering that no
# matter where our kernel assumes it is, it will always be loaded somewhere.
# Up until recently, we had been loading ourselves to te 1mb (0x100000) area,
# because the area 0x0-0xFFFFF is reserved for BIOS and other things.
# So our kernel will keep being loaded to 0x100000, because we set it that
# way in the linker script. We could easily choose to map 0x100000 to
# 0xC0000000 and be done with it. And, indeed, it would work. But we would
# be losing access to the first mb of memory (because it would be left
# unmapped). Yet, we use the first mb of memory for things such as VRAM, through
# memory-mapped IO. This means that it will also be beneficial for us to map
# something to the 0-1mb range. This would allow us to use it, without having
# any pointers to it. We leave the 0-1mb 'virtual' range free, but have another
# virtual area which points to it, and allows us to access it.
# 
# One such way to accomplish both these things: map the kernel and the 0-1mb
# area, is precisely by mapping them all together at once! Our kernel starts
# at 0x100000, and it goes to 0x100000+kernel_size. So, if we map
# 0x0--0x100000+kernel_size all in one go, we can solve both our problems.
# Now, of course we could just map 0x0 to any other address, even to
# 0xC0100000 itself, but if we choose to map 0x0 to 0xC0000000 (meaning that
# our kernel, which is loaded at 0x100000 is indeed mapped to 0xC0100000),
# then we have a pretty value which we can use as offset, and which doesn't
# really interfere with our programming. For instance, VRAM moves from
# 0xB8000 to 0xC00B8000. The fact that 0xC0000000 shares no non-zero bits
# with 0x100000 allows us to quickly convert from 0x0-0x100000 to 
# 0xC0000000-0xC0100000, while still remaining right above the 3gb barrier,
# which was our initial goal. It's a very personal and subjective thing,
# really.)
# 
#
# It might be relevant to note that we are using big pages to do this,
# so we use 4mb pages instead of 4kb pages. This eases coding of the
# page directory, but it also means that we end up identity-mapping 0-4mb
# and not 0-1mb. This is okay, but I thought I should point it out.
#
#
# Further reading:
# * http://wiki.osdev.org/Higher_Half_Kernel (theory)
# * http://wiki.osdev.org/Higher_Half_bare_bones (practice)
# * http://wiki.osdev.org/User:Mduft/HigherHalf_Kernel_with_32-bit_Paging (more practice)
# * http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html (Another way to look at things)
# * http://www.brokenthorn.com/Resources/OSDev18.html (more focused on paging)
#
# ----------------------------------------------------------------------------
#


# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:


# This is the virtual base address of kernel space. It must be used to convert virtual
# addresses into physical addresses until paging is enabled. Note that this is not
# the virtual address where the kernel image itself is loaded -- just the amount that must
# be subtracted from a virtual address to get a physical address.
.set KERNEL_VIRTUAL_BASE, 0xC0000000                  # 3GB
.set KERNEL_PAGE_NUMBER, (KERNEL_VIRTUAL_BASE >> 22) # Page directory index of kernel's 4MB PTE.

# The PMM will be expecting 0xB0000000 to be mapped, and in fact, indetity-mapped,
# to do its work, so we have to do it here. Do note that the only reason
# why it needs to be identity-mapped is because the VMM needs the PMM to
# give it physical addresses (so as to use them in Page Directories and Page Tables).
.set PMM_VIRTUAL_BASE, 0xB0000000               
.set PMM_PAGE_NUMBER, (PMM_VIRTUAL_BASE >> 22) # Page directory index of kernel's 4MB PTE.

# Declares the boot Paging directory to load a virtual higher half kernel
.section .data
.align 0x1000
.global _boot_page_directory
_boot_page_directory:
    .long 0x00000083
    .fill (KERNEL_PAGE_NUMBER - 1), 4, 0x00000000
    .long 0x00000083
    .fill (1024 - KERNEL_PAGE_NUMBER - 1), 4, 0x00000000

.section .text
.global _loader
_loader:
  # Load Page Directory Base Register. Until paging is set up, the code must
  # be position-independent and use physical addresses, not virtual ones
  mov $(_boot_page_directory - KERNEL_VIRTUAL_BASE), %ecx
  mov %ecx, %cr3

  # Set PSE bit in CR4 to enable 4MB pages.
  mov %cr4, %ecx
  or $0x00000010, %ecx
  mov %ecx, %cr4

  # Set PG bit in CR0 to enable paging.
  mov %cr0, %ecx
  or $0x80000000, %ecx
  mov %ecx, %cr0

  # Start fetching instructions in kernel space.
  # Since eip at this point holds the physical address of this command
  # (approximately 0x00100000) we need to do a long jump to the correct
  # virtual address of _start which is approximately 0xC0100000.
  movl $_start, %edx
  jmp *%edx


# The Higher-Half Kernel entry point.
.global _start
.type _start, @function
_start:

  # Unmap the identity-mapped first 4MB of physical address space.
  # movl $0, (_boot_page_directory) #uncommenting this crashes all, figure why
  invlpg (0)

  # Enter protection mode
  mov %cr0, %eax
  or $1, %al
  mov %eax, %cr0

  # Push multiboot info
  movl $stack_top, %esp
  push %ebx


  # Initialize the core kernel before running the global constructors.
  call kernel_early

  # Call the global constructors.
  call _init

  # Transfer control to the main kernel.
  call kernel_main

  # Hang if kernel_main unexpectedly returns.
  cli
.Lhang:
  hlt
  jmp .Lhang
.size _start, . - _start