//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

//
// This  code  is  responsible of loading  the kernel and  copying it  to it's
// final location in memory, and then transferring control to it.
//

#include "Bootstrap.h"

.code16

//==============================================================================
// First, get the parameters that were passed to us on the stack.
    
    // Pop the kernel size
    popw    %ax
    movw    %ax, kernel_size

    // Pop the current disk position
    popw    %ax
    movb    %al, sector
    popw    %ax
    movb    %al, head
    popw    %ax
    movw    %ax, cylinder

    // Pop the drive geometry
    popw    %ax
    movb    %al, max_sector
    popw    %ax
    movb    %al, max_head
    popw    %ax
    movw    %ax, max_cylinder

    // Pop the boot BIOS drive number
    popw    %ax
    movb    %al, drive

//==============================================================================
// An annoying thing we have to do is to  enable the dreaded A20 line in order
// to  be able  to access memory upper than 1 mb. There is plenty of different
// ways to do that, and not all  computers support all of  them, so this  is a
// pain.

    // FIRST METHOD ------------------------------------------------------------

    // Attempt to enable the A20 line using a BIOS call (not always working)
    movw    $0x2401, %ax
    int     $0x15
    jnc     gate_a20_succeeded

    // SECOND METHOD -----------------------------------------------------------

    // Wait for the keyboard controller to be ready
    call    wait_for_keyboard_controller

    // Tell the controller we're gonna send a command
    movb    $0xD1, %al
    outb    %al, $0x64
    call    wait_for_keyboard_controller
    
    // Enable the A20 line
    movb    $0xDF, %al
    outb    %al, $0x60
    call    wait_for_keyboard_controller

gate_a20_succeeded:

    // TODO: Implement other methods of doing that, look at :
    // http://www.win.tue.nl/~aeb/linux/kbd/A20.html

//==============================================================================
// Now, we must load the kernel and put it at it's final location.

    // Trace what we're doing
    movw    $msg_loading_kernel, %ax
    call    write_string

    // Initialize the destination address for kernel sectors
    movl    $KERNEL_LOAD_ADDRESS, current

read_kernel_again:

        // Read the current sector into memory
        movb    $0x2, %ah
        movb    $1, %al
        movb    cylinder, %ch
        movb    head, %dh
        movb    sector, %cl
        movb    drive, %dl
        movw    %ds, %bx
        movw    %bx, %es
        movw    $0, %bx
        int     $0x13

        // Check if the read was performed correctly
        jnc     read_kernel_ok

        // An error occured.  Recalibrate the disk drive and try again.
        movb    $0, %ah
        movb    drive, %dl
        int     $0x13
        jmp     read_kernel_again

    read_kernel_ok:

        // Increment the sector
        movb    max_sector, %al
        cmpb    sector, %al
        je      increment_head

        incb    sector
        jmp     increment_done

    increment_head:

        // Increment the head
        movb    $1, sector

        movb    max_head, %al
        cmpb    head, %al
        je      increment_cylinder

        incb    head
        jmp     increment_done

    increment_cylinder:

        // Increment the cylinder
        movb    $0, head
        incw    cylinder

    increment_done:

        // Disable interrupts
        cli

        // Initialize registers to do the copy operation
        movl    $BOOTSTRAP_SEGMENT * 16, %esi
        movl    current, %edi
        movl    $0x80, %ecx

        // Advance /current/
        addl    $0x200, current

        // Load the protected mode GDT
        lgdtl   lgdt_params

        // Enter protected mode
        movl    %cr0, %eax
        orl     $0x1B, %eax
        movl    %eax, %cr0

        // Serialize the processor
        jmp     read_kernel_serialize_1

    read_kernel_serialize_1:

        // Load %es with the selector pointing to the whole memory
        movw    $16, %ax
        movw    %ax, %es

    read_kernel_copy_again:

            // Copy a long
.code16gcc
            movl    %es:(%esi), %eax
            movl    %eax, %es:(%edi)
.code16

            // Increment pointers
            addl    $4, %esi
            addl    $4, %edi

            // Loop
            loopl   read_kernel_copy_again

        // Return to real mode
        movl    %cr0, %eax
        andl    $~0x1B, %eax
        movl    %eax, %cr0

        // Serialize the processor
        jmp     read_kernel_serialize_2

    read_kernel_serialize_2:

        // Reset %es so it doesn't address the whole memory anymore
        movw    $0x9000, %ax
        movw    %ax, %es

        // Re-enable interrupts
        sti

        // Trace progress every 32 blocks
        movw    kernel_size, %ax
        andw    $0xFF30, %ax
        cmpw    kernel_size, %ax
        jne     read_kernel_do_not_trace
        movw    $msg_progress, %ax
        call    write_string

    read_kernel_do_not_trace:

        // Check if there is still sectors to read
        decw    kernel_size
        movw    $0, %ax
        cmpw    kernel_size, %ax
        je      read_kernel_done

        // Loop for the next sector
        jmp     read_kernel_again

read_kernel_done:

    // Stop the floppy drive's motor
    movb    $0, %ah
    movb    drive, %dl
    int     $0x13

    // Trace what we're done
    movw    $msg_ok, %ax
    call    write_string

//==============================================================================
// We must prepare a set  of page tables that  will map memory  locations used
// by the kernel to boot (until he gets it's own memory manager up).

    // Initialize registers for building the page tables
    //
    //  %es  - The real address segment of the current page table entry.
    //  %di  - The real address offset of the current page table entry.
    //  %edx - The current address that the page table entry points to.

    movw    $PAGE_TABLES_SEGMENT, %ax
    movw    %ax, %es
    movw    $0, %di

    // We first build a single page table  that points to the first 4mb of the
    // physical memory (useful for the initial console, etc.)

    movl    $0, %edx

build_page_table_again_1:

        // Build the current page table entry
        movl    %edx, %eax
        orl     $3, %eax
        movl    %eax, %es:(%di)

        // Increment counters
        addl    $0x1000, %edx
        addw    $4, %di

        // Loop if needed
        cmp     $0x1000, %di
        jne     build_page_table_again_1

    // We then use the remaining of  the segment to  build  pages  tables that
    // maps  the kernel  image and the  memory that follow (this leaves enough
    // room for a 60mb kernel image, which is largely enough!)

    movl    $KERNEL_LOAD_ADDRESS, %edx

build_page_table_again_2:

        // Build the current page table entry
        movl    %edx, %eax
        orl     $3, %eax
        movl    %eax, %es:(%di)

        // Increment counters
        addl    $0x1000, %edx
        addw    $4, %di

        // Loop if needed (exit when we wrap over)
        cmp     $0, %di
        jne     build_page_table_again_2

//==============================================================================
// We have to build a page directory that references the tables we created.

    // Initialize registers for building the page directory
    //
    //  %es  - The real address segment of the current page directory entry.
    //  %di  - The real address offset of the current page directory entry.
    //  %edx - The address of the current page table.

    movw    $PAGE_DIRECTORY_SEGMENT, %ax
    movw    %ax, %es
    movw    $0, %di
    movl    $(PAGE_TABLES_SEGMENT * 16), %edx

    // Go through all page directory entries and initialize them properly

build_page_directory_again:

        // Initialize the page directory entry to 0
        movl    $0, %es:(%di)

        // Check if the current page directory entry is the first
        cmpw    $0, %di
        jne     build_page_directory_not_first

        // Make  the  page directory point to  the first  page table  (the one
        // that maps the first 4mb of physical memory)
        movl    %edx, %eax
        orl     $3, %eax
        movl    %eax, %es:(%di)

        // Increment to the first kernel page table
        addl    $0x1000, %edx

    build_page_directory_not_first:

        // Check if the current page directory entry is a kernel one
        cmpw    $KERNEL_VIRTUAL_ADDRESS >> 20, %di
        jl      build_page_directory_not_kernel
        cmpw    $KERNEL_VIRTUAL_ADDRESS >> 20 + 60, %di
        jg      build_page_directory_not_kernel

        // Make the page directory point to a kernel page table
        movl    %edx, %eax
        orl     $3, %eax
        movl    %eax, %es:(%di)

        // Increment to the next kernel page table
        addl    $0x1000, %edx

    build_page_directory_not_kernel:

        // Increment counters
        addw    $4, %di

        // Loop if needed
        cmp     $0x1000, %di
        jne     build_page_directory_again

//==============================================================================
// The kernel is loaded, all that remains is to transfer control to it.

    // Disable interrupts
    cli

    // Load the protected mode GDT
    lgdtl   lgdt_params

    // Load the page directory for the kernel
    movl    $(PAGE_DIRECTORY_SEGMENT * 16), %eax
    movl    %eax, %cr3

    // Enter protected mode with paging enabled
    movl    %cr0, %eax
    orl     $0x80000001, %eax
    movl    %eax, %cr0

    // Serialize the processor
    jmp     boot_kernel_serialize

boot_kernel_serialize:

    // Load %ds and %ss with the selector pointing to the whole memory
    movw    $16, %ax
    movw    %ax, %ds
    movw    %ax, %ss

    // Put the stack pointer at the end of base memory
    movl    $0xA0000, %eax
    movl    %eax, %esp
   
    // Far jump to the kernel entry point
    ljmpl   $8, $KERNEL_VIRTUAL_ADDRESS 

//******************************************************************************
// Waits for the keyboard controller to be ready.
//******************************************************************************

wait_for_keyboard_controller:

    // Preserve registers we'll use
    pushl   %eax

    wait_for_keyboard_controller_again:

        // Check if the controller is ready
        in      $0x64, %al
        testb   $2, %al

        // If it isn't, try another time
        jnz     wait_for_keyboard_controller_again

    // Restore the registers
    popl    %eax

    ret

//******************************************************************************
// Writes a character string to the console.
//
// Parameters:
//  %ax - Pointer to the character string to write.
//******************************************************************************

write_string:

    // Preserve registers we'll use
    pushl   %eax
    pushl   %ebx
    pushl   %esi

    // Initialize registers
    movw    %ax, %si

    write_string_again:

        // If the current character is '\0', break the loop
        cmpb    $0, (%si)
        je      write_string_done

        // Write the current character
        movb    $0xE, %ah
        movb    (%si), %al
        movw    $0, %bx
        int     $0x10

        // Advance the pointer and loop
        inc     %si
        jmp     write_string_again

    write_string_done:

    // Restore the registers
    popl    %esi
    popl    %ebx
    popl    %eax

    ret

//******************************************************************************
// Writes an hexadecimal character to the console.
//
// Parameters:
//  %eax - The hexadecimal to write.
//******************************************************************************

write_hexadecimal:

    // Preserve registers we'll use
    pushl   %eax
    pushl   %ebx
    pushl   %ecx

    // Initialize registers
    movw    $8, %cx

    write_hexadecimal_again:
        
        // Compute the current digit value
        movl    %eax, %ebx
        andl    $0xF0000000, %ebx
        shrl    $28, %ebx

        // Write  the current digit.  The  conversion  code to  hexadecimal is
        // borrowed from  the linux  kernel code. I have no idea why it works,
        // but it does !
        pushl   %eax
        movb    $0xE, %ah
        movb    %bl, %al
        addb    $0x90, %al
        daa
        adc     $0x40, %al
        daa
        movw    $0, %bx
        int     $0x10
        popl    %eax

        // Loop
        shll    $4, %eax
        loopw   write_hexadecimal_again

    // Restore the registers
    popl    %ecx
    popl    %ebx
    popl    %eax

    ret

//******************************************************************************
// Variables.
//******************************************************************************

// The boot device's BIOS drive number
drive:                  .byte   0

// The disk geometry
max_cylinder:           .word   0
max_head:               .byte   0
max_sector:             .byte   0

// The current sector address
cylinder:               .word   0
head:                   .byte   0
sector:                 .byte   2

// The size of the kernel in sectors
kernel_size:            .word   0

// Counter used for various operations
current:                .long   0

// The GDT used to copy memory and boot the kernel
.align 4
gdt:                    .long   0x00000000
                        .long   0x00000000
                        .long   0x0000FFFF
                        .long   0x00CF9800
                        .long   0x0000FFFF
                        .long   0x00CF9200
    
// The parameters used to call LGDT
.align 4
lgdt_params:            .word   23
                        .long   gdt + BOOTSTRAP_SEGMENT * 16

// The messages used throughout the process
msg_loading_kernel:     .asciz  "Loading kernel"
msg_progress:           .asciz  "."
msg_ok:                 .asciz  " OK\n\r"
