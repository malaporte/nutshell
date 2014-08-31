//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/Paging.h"
#include "Intel386/Intel386.h"
#include "Intel386/PageDirectory.h"

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// Allocates a new page directory descriptor.
//
// Returns:
//  The page directory descriptor.
//******************************************************************************
size_t AllocatePageDirectoryDescriptor()
{
    // Allocate a new empty page directory
    PageDirectory* pDirectory(new (memalign(OS_PAGE_SIZE, sizeof(PageDirectory))) PageDirectory());

    return reinterpret_cast<size_t>(pDirectory);
}

//******************************************************************************
// Releases a page directory descriptor.
//
// Parameters:
//  p_Directory - The page directory descriptor.
//******************************************************************************
void ReleasePageDirectoryDescriptor(size_t p_Directory)
{
    // Convert the descriptor to a pointer to a page directory
    PageDirectory* pDirectory = reinterpret_cast<PageDirectory*>(p_Directory);

    // Release the page directory
    delete pDirectory;
}

//******************************************************************************
// Dumps the content of a page directory to the console.
//
// Parameters:
//  p_Directory - The page directory descriptor.
//******************************************************************************
void DumpPageDirectoryDescriptor(size_t p_Directory)
{
    // Display the title
    *g_pConsole << "#Dumping page directory descriptor " << p_Directory << ":#\n";

    // Convert the descriptor to a pointer to a page directory
    PageDirectory* pDirectory = reinterpret_cast<PageDirectory*>(p_Directory);

    // Go through all page tables within the directory
    for (size_t i = 0; i < PAGE_DIRECTORY_CAPACITY; ++i) {
        // If the page table isn't there skip it
        if (!(*pDirectory)[i].P()) continue;

        // Convert the base field to a pointer to a page table
        PageTable* pTable = reinterpret_cast<PageTable*>((*pDirectory)[i].Base());

        // Go through all pages within the directory
        for (size_t j = 0; j < PAGE_TABLE_CAPACITY; ++j) {
            // If the page isn't present skip it
            if (!(*pTable)[j].P()) continue;

            // Dump the page information
            *g_pConsole << i * PAGE_TABLE_CAPACITY + j * OS_PAGE_SIZE << " -> " << static_cast<size_t>((*pTable)[j].Base());
            *g_pConsole << "\n";
        }

        break;
    }
}

//******************************************************************************
// Map a page table to a page directory.
//
// Parameters:
//  p_Directory  - The descriptor of the page directory to map to.
//  p_Table      - The descriptor of the page table to map.
//  p_Index      - The index where to map the page table.
//******************************************************************************
void MapPageTableToDirectory(size_t p_Directory, size_t p_Table, size_t p_Index)
{
    assert(p_Directory != 0);
    assert(p_Table != 0);
    assert(p_Index < PAGE_DIRECTORY_CAPACITY);

    // Convert the descriptor to a pointer to a page directory
    PageDirectory* pDirectory = reinterpret_cast<PageDirectory*>(p_Directory);

    // Map the specified page table within the directory
    (*pDirectory)[p_Index].Base(GetPhysicalAddress(reinterpret_cast<PageTable*>(p_Table)));
    (*pDirectory)[p_Index].P(true);
    (*pDirectory)[p_Index].RW(true);
}

//******************************************************************************
// Unmaps a page table from a page directory.
//
// Parameters:
//  p_Directory  - The descriptor of the page directory to unmap from.
//  p_Index      - The index of the page table to unmap.
//******************************************************************************
void UnmapPageTableFromDirectory(size_t p_Directory, size_t p_Index)
{
    assert(p_Directory != 0);
    assert(p_Index < PAGE_DIRECTORY_CAPACITY);

    // Convert the descriptor to a pointer to a page directory
    PageDirectory* pDirectory = reinterpret_cast<PageDirectory*>(p_Directory);

    // Unmap the specified page table
    (*pDirectory)[p_Index].Reset();
}

//******************************************************************************
// Allocates a new page table descriptor.
//
// Returns:
//  The page table descriptor.
//******************************************************************************
size_t AllocatePageTableDescriptor()
{
    // Allocate a new empty page table
    PageTable* pTable(new (memalign(OS_PAGE_SIZE, sizeof(PageTable))) PageTable());

    return reinterpret_cast<size_t>(pTable);
}

//******************************************************************************
// Releases a page table descriptor.
//
// Parameters:
//  p_Table - The page table descriptor.
//******************************************************************************
void ReleasePageTableDescriptor(size_t p_Table)
{
    // Convert the descriptor to a pointer to a page table
    PageTable* pTable = reinterpret_cast<PageTable*>(p_Table);

    // Release the page table
    delete pTable;
}

//******************************************************************************
// Map a page to a frame within a page table.
//
// Parameters:
//  p_Table - The descriptor of the page table that contains the page.
//  p_Page  - The index of the page to map.
//  p_Frame - The index of the frame to map the page to.
//******************************************************************************
void MapPageToFrame(size_t p_Table, size_t p_Page, size_t p_Frame)
{
    assert(p_Table != 0);
    assert(p_Page < PAGE_TABLE_CAPACITY);
    assert(p_Frame < g_MemorySize / OS_PAGE_SIZE);

    // Convert the descriptor to a pointer to a page table
    PageTable* pTable = reinterpret_cast<PageTable*>(p_Table);

    // Map the specified frame to the page
    (*pTable)[p_Page].Base(p_Frame * OS_PAGE_SIZE);
    (*pTable)[p_Page].P(true);
    (*pTable)[p_Page].RW(true);
}

//******************************************************************************
// Unmaps a page from a page table from it's associated frame.
//
// Parameters:
//  p_Table - The descriptor of the page table that contains the page.
//  p_Page  - The index of the page to unmap.
//******************************************************************************
void UnmapPageFromFrame(size_t p_Table, size_t p_Page)
{
    assert(p_Table != 0);
    assert(p_Page < PAGE_TABLE_CAPACITY);

    // Convert the descriptor to a pointer to a page table
    PageTable* pTable = reinterpret_cast<PageTable*>(p_Table);

    // Reset the page table entry
    (*pTable)[p_Page].Reset();
}

//******************************************************************************
// Resets the accessed flag of a page.
//
// Parameters:
//  p_Table - The descriptor of the page table that contains the page.
//  p_Page  - The index of the page to unmap.
//
// Returns:
//  Whether the flag was previously set.
//******************************************************************************
bool ResetPageAccessedFlag(size_t p_Table, size_t p_Page)
{
    assert(p_Table != 0);
    assert(p_Page < PAGE_TABLE_CAPACITY);
   
    // Convert the descriptor to a pointer to a page table
    PageTable* pTable = reinterpret_cast<PageTable*>(p_Table);

    // Keep the old value of the flag and reset it
    bool accessed = (*pTable)[p_Page].A();
    (*pTable)[p_Page].A(false);

    return accessed;
}

//******************************************************************************
// Invalidates the TLB entry for a specific page.
//
// Parameters:
//  p_Table     - The index of the page table within the page directory.
//  p_Page      - The index of the page with the page table.
//******************************************************************************
void InvalidateTLBEntry(size_t p_Table, size_t p_Page)
{
    assert(p_Table < PAGE_DIRECTORY_CAPACITY);
    assert(p_Page < PAGE_TABLE_CAPACITY);
   
    // Invalidate the TLB entry for the given page
    void* entry = reinterpret_cast<void*>((p_Table * PAGE_TABLE_CAPACITY + p_Page) * OS_PAGE_SIZE);
    asm("invlpg %0" : : "m" (entry));
}

//******************************************************************************
// Converts a pointer into a physical address.
//
// Parameters:
//  p_pPointer - The pointer to convert.
//
// Returns:
//  The physical address associated with the pointer.
//******************************************************************************
size_t GetPhysicalAddress(void* p_pPointer)
{
    // Compute the index of the page table and page that contains the address
    size_t table = reinterpret_cast<size_t>(p_pPointer) / PAGE_TABLE_SIZE;
    size_t page  = (reinterpret_cast<size_t>(p_pPointer) % PAGE_TABLE_SIZE) / OS_PAGE_SIZE;

    // Retrieve the current page directory
    PageDirectory* pDirectory;
    asm("movl %%cr3, %0" : "=r" (pDirectory));

    // The page table for the address should be present!
    assert((*pDirectory)[table].P());

    // Retrieve the page table for the address
    PageTable* pTable = reinterpret_cast<PageTable*>((*pDirectory)[table].Base());

    // Ensure the page for the address is present
    assert((*pTable)[page].P());

    return (*pTable)[page].Base() + reinterpret_cast<size_t>(p_pPointer) % OS_PAGE_SIZE;
}

} // namespace Intel386
} // namespace Nutshell
