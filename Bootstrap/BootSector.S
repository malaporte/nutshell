//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

//
// This code  is the one  loaded by the  bios at boot time. It must not exceed
// 512 bytes in size because it have to fit on only one disk sector.
//

#include "Bootstrap.h"

//******************************************************************************
// This is were we get the control from the BIOS.  We expect the following :
//  cs:ip   - Points to the beginning of the boot sector code (0x0000:0x7C00).
//  dl      - The BIOS drive number of the boot device.
//  es:si   - Points to the partition table entry.
//******************************************************************************

.code16                                         

//==============================================================================
// First of all we must get ourselves out  of the way  for others boot sectors
// and establish a valid working environment.

    // Check if we already initialized ourselves
    movw    %cs, %ax
    cmpw    $BOOTSTRAP_SEGMENT, %ax
    je      already_initialized

    // Copy ourselves into the base segment
    movw    $0, %ax
    movw    %ax, %ds
    movw    $BOOTSTRAP_SEGMENT, %ax
    movw    $0x7C00, %si
    movw    %ax, %es
    movw    $0, %di
    movw    $512, %cx
    rep     movsb

    // Initialize the data segment register
    movw    $BOOTSTRAP_SEGMENT, %ax
    movw    %ax, %ds

    // Initialize the stack segment register
    movw    $BOOTSTRAP_SEGMENT, %ax
    movw    %ax, %ss
    movw    $0xFFFF, %sp

    // Enable interrupts
    sti

    // Do a far return to the new segment
    pushw   $BOOTSTRAP_SEGMENT
    pushw   $0
    lret

already_initialized:

//==============================================================================
// We must now retrieve the boot drive geometry to be able to load the kernel
// loader code.

    // Keep the boot device's drive number
    movb    %dl, drive

    // Preserve es:si in case we need them later (points to the partition table)
    pushw   %es
    pushw   %si

    // Call the BIOS to return the boot device's geometry
    movb    $0x8, %ah
    movb    drive, %dl
    int     $0x13

    // Keep the cylinder count
    movb    %ch, max_cylinder
    movb    %cl, max_cylinder + 1
    shrb    $6, max_cylinder + 1

    // Keep the head count
    movb    %dh, max_head

    // Keep the sector count
    andb    $63, %cl
    movb    %cl, max_sector

    // Restore %es and %si
    pop     %si
    pop     %es

//==============================================================================
// If we're booting from a hard drive, we must do some additionnal processing.

    // Check if this is a hard drive boot
    cmpb    $0x80, drive
    jg      not_from_hard_drive

    // Get the starting sector from the partition table
    movw    %es:0x8(%si), %ax
    movw    %es:0xA(%si), %dx

    // Compute the starting cylinder
    movw    max_cylinder, %bx
    divw    %bx
    movw    %ax, cylinder
    movw    %dx, %ax

    // Compute the starting head
    movb    max_head, %bl
    divb    %bl
    movb    %al, head

not_from_hard_drive:

//==============================================================================
// We're now ready to load the kernel loader.  We assume it is located right
// after us on the boot disk.

    // This variable has been filled by the tool that made the floppy image
    movw    kernel_loader_size, %cx

read_loop:

    read_again:

        // Preserve cx
        pushw   %cx

        // Read the current sector into memory
        movb    $0x2, %ah
        movb    $1, %al
        movb    cylinder, %ch
        movb    head, %dh
        movb    sector, %cl
        movb    drive, %dl
        movw    %ds, %bx
        movw    %bx, %es
        movw    destination, %bx
        int     $0x13

        // Restore cx
        popw    %cx

        // Check if the read was performed correctly
        jnc     read_ok

        // An error occured.  Recalibrate the disk drive and try again.
        movb    $0, %ah
        movb    drive, %dl
        int     $0x13

        jmp     read_again

    read_ok:

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

        // Increment the destination address
        addw    $512, destination

        // Loop
        loopw   read_loop

//==============================================================================
// The kernel loader is now loaded right after us into memory.  We can jump to it.

    // Push the boot BIOS drive number
    movb    drive, %al
    pushw   %ax

    // Push the drive geometry
    movw    max_cylinder, %ax
    pushw   %ax
    movb    max_head, %al
    pushw   %ax
    movb    max_sector, %al
    pushw   %ax

    // Put the current disk position
    movw    cylinder, %ax
    pushw   %ax
    movb    head, %al
    pushw   %ax
    movb    sector, %al
    pushw   %ax

    // Put the kernel size
    movw    kernel_size, %ax
    pushw   %ax

    // Do a far return to the kernel loader
    pushw   $BOOTSTRAP_SEGMENT
    pushw   $512
    lret

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

// The current destination for read operations
destination:            .word   0x200

// Those are filled by an external tool
.org 506
kernel_loader_size:     .word   0
kernel_size:            .word   0

// Advance to byte 510 and put the signature
.org 510
.word 0x55AA
