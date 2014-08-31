//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_PAGING_H
#define INTEL386_PAGING_H

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// Paging related constants
//******************************************************************************

// The capacity of the various objects
#define PAGE_DIRECTORY_CAPACITY 1024
#define PAGE_TABLE_CAPACITY     1024

// The size in bytes of the various objects
#define OS_PAGE_SIZE            4096
#define PAGE_TABLE_SIZE         (OS_PAGE_SIZE * PAGE_TABLE_CAPACITY)
#define PAGE_DIRECTORY_SIZE     (PAGE_TABLE_SIZE * PAGE_DIRECTORY_CAPACITY)

//******************************************************************************
// Functions used to manipulate the paging functionnalities.
//******************************************************************************

size_t  AllocatePageDirectoryDescriptor();
void    ReleasePageDirectoryDescriptor(size_t p_Directory);
void    DumpPageDirectoryDescriptor(size_t p_Directory);
void    MapPageTableToDirectory(size_t p_Directory, size_t p_Table, size_t p_Index);
void    UnmapPageTableFromDirectory(size_t p_Directory, size_t p_Index);
size_t  AllocatePageTableDescriptor();
void    ReleasePageTableDescriptor(size_t p_Table);
void    MapPageToFrame(size_t p_Table, size_t p_Page, size_t p_Frame);
void    UnmapPageFromFrame(size_t p_Table, size_t p_Page);
bool    ResetPageAccessedFlag(size_t p_Table, size_t p_Page);
void    InvalidateTLBEntry(size_t p_Table, size_t p_Page);
size_t  GetPhysicalAddress(void* p_pPointer);

} // namespace Intel386
} // namespace Nutshell

#endif // !INTEL386_PAGING_H
