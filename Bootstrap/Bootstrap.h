//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef BOOTSTRAP_BOOSTRAP_H
#define BOOTSTRAP_BOOSTRAP_H

/*

Real mode memory layout :

SEGMENT                 CONTENT
================================================================================
0x0000 ... 0x1000       BIOS code and data.
0x1000 ... 0x1020       Boot sector code.
0x1020 ... 0x2000       Kernel loader code and stack.
0x2000 ... 0x3000       Kernel page tables.
0x3000 ... 0x3100       Kernel page directory.

*/

#define BOOTSTRAP_SEGMENT       0x1000
#define PAGE_TABLES_SEGMENT     0x2000
#define PAGE_DIRECTORY_SEGMENT  0x3000

/*

Protected mode memory layout :

ADDRESS                 CONTENT
================================================================================
0x100000                Kernel image.

*/

#define KERNEL_LOAD_ADDRESS     0x100000
#define KERNEL_VIRTUAL_ADDRESS  0xC0000000

#endif // BOOTSTRAP_BOOSTRAP_H
