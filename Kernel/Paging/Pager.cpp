//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Machine.h"
#include "Paging/Pager.h"
#include "Threading/Scheduler.h"
#include "Threading/InterruptLock.h"

namespace Nutshell {
namespace Paging {

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Pager class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//******************************************************************************
Pager::Pager()
:   m_KernelSize(0)
{
    assert(this != 0);

    // Initialize the frame vector with empty frames
    for (size_t i = 0; i < Machine::g_MemorySize / OS_PAGE_SIZE; ++i) {
        m_Frames.push_back(Frame());
        m_Frames.back().m_pOwner = 0;
    }

    // Preallocate page  tables for  the kernel  memory (we allocate enough to
    // cover  the  whole size of the physical memory, since the  kernel cannot
    // grow bigger than this hard limit...)
    for (size_t i = 0; i < Machine::g_MemorySize / PAGE_TABLE_SIZE; ++i) {
        m_KernelTables.push_back(Machine::AllocatePageTableDescriptor());
    }

    // Calls to KernelSize  must  never generate memory allocations,  and thus
    // we must ensure here that the sequences  they  use  already  have enough
    // memory allocated for their  use.  The  page table vector is  already at
    // it's  full size, but we must take care of the frame vector.
    m_KernelFrames.reserve(Machine::g_MemorySize / OS_PAGE_SIZE);

    // Put  the  frames for the  whole available  memory into the not recently
    // accessed  deque. TODO:  Preserve the  frames  that we should  not erase
    // (BIOS code, data and interrupt table, along with video memory, etc.)
    for (size_t i = KERNEL_LOAD_ADDRESS; i < Machine::g_MemorySize; i += OS_PAGE_SIZE) {
        m_NotRecent.push_back(i / OS_PAGE_SIZE);
    }

    // Now fill the vector  of frames available  for the kernel  with those it
    // currently uses. It must be  in an order so that  /KernelSize/ retrieves
    // and map them  correctly  to  the  corresponding  address.  We must also
    // remove those frames from the not recently accessed ones.
    for (size_t i = KERNEL_SPACE_BOUNDARY; i < reinterpret_cast<size_t>(sbrk(0)); i += OS_PAGE_SIZE) {
        size_t frame = Machine::GetPhysicalAddress(reinterpret_cast<void*>(i)) / OS_PAGE_SIZE;
        m_KernelFrames.push_back(frame);
        FrameIndexDeque::iterator found = std::find(m_NotRecent.begin(), m_NotRecent.end(), frame);
        if (found != m_NotRecent.end()) m_NotRecent.erase(found);
    }
    
    // The elements are in the inverse order as they should be...
    std::reverse(m_KernelFrames.begin(), m_KernelFrames.end()); 

    // Initialize the kernel size using the value returned by sbrk()
    KernelSize(reinterpret_cast<size_t>(sbrk(0)) - KERNEL_SPACE_BOUNDARY);
    assert(m_KernelFrames.empty());
    PrepareNextKernelSize();
}

//******************************************************************************
// Destructor.
//******************************************************************************
Pager::~Pager()
{
    assert(this != 0);

    // The pager should never be brought down...
    assert(false);
}

//******************************************************************************
// Adds a pageable to the pager.
//
// Parameters:
//  p_spPageable - The pageable to add to the pager.
//******************************************************************************
void Pager::AddPageable(PageableSP p_spPageable)
{
    assert(this != 0);
    assert(p_spPageable != 0);
    Threading::InterruptLock intlock;
    Threading::SpinLockLocker lock1(m_SpinLock);
    Threading::SpinLockLocker lock2(p_spPageable->m_SpinLock);
    assert(!Utilities::SequenceContains(m_Pageables, p_spPageable));

    // Add the pageable to the vector
    m_Pageables.push_back(p_spPageable);

    // Map  the  kernel  page  tables  within  the  pageable.  They  have been
    // prepared  by  the  constructor and  the  /KernelSize/  method  and they
    // contains pages that map the kernel memory.
    for (size_t i = 0; i * PAGE_TABLE_CAPACITY < m_KernelSize; ++i) {
        Machine::MapPageTableToDirectory(p_spPageable->m_Directory, m_KernelTables[i], KERNEL_SPACE_BOUNDARY / PAGE_TABLE_SIZE + i);
    }
}

//******************************************************************************
// Removes a pageable from the pager.
//
// Parameters:
//  p_spPageable - The pageable to remove from the pager.
//******************************************************************************
void Pager::RemovePageable(PageableSP p_spPageable)
{
    assert(this != 0);
    assert(p_spPageable != 0);
    Threading::InterruptLock intlock;
    Threading::SpinLockLocker lock1(m_SpinLock);
    Threading::SpinLockLocker lock2(p_spPageable->m_SpinLock);
    assert(Utilities::SequenceContains(m_Pageables, p_spPageable));

    // Unmap the kernel page tables from the pageable
    for (size_t i = 0; i * PAGE_TABLE_CAPACITY < m_KernelSize; ++i) {
        Machine::UnmapPageTableFromDirectory(p_spPageable->m_Directory, KERNEL_SPACE_BOUNDARY / PAGE_TABLE_SIZE + i);
    }

    // Remove the pageable from the vector
    m_Pageables.erase(std::find(m_Pageables.begin(), m_Pageables.end(), p_spPageable));
}

//******************************************************************************
// Locks a mapable into physical memory.
//
// Parameters:
//  p_spMapable - The mapable to lock into memory.
//  p_Address   - The physical address at which to lock the mapable. 
//******************************************************************************
void Pager::LockMapable(MapableSP p_spMapable, size_t p_Address)
{
    assert(this != 0);
    assert(p_spMapable != 0);
    assert(p_Address % OS_PAGE_SIZE == 0 || p_Address == 0xFFFFFFFF);
    Threading::InterruptLock intlock;
    Threading::SpinLockLocker lock1(m_SpinLock);
    Threading::SpinLockLocker lock2(p_spMapable->m_SpinLock);
    assert(!p_spMapable->m_Locked);

    // Go through all the pages of the mapable and ensure they are in memory
    for (Mapable::PageVector::iterator it = p_spMapable->m_Pages.begin(); it != p_spMapable->m_Pages.end(); ++it) {
        // First we must determine which frame the current page will use.

        // Check if the location where we should lock the page is defined
        if (p_Address != 0xFFFFFFFF) {
            // Compute the frame that this page will use and ensure it's available
            it->m_Address = p_Address / OS_PAGE_SIZE + (it - p_spMapable->m_Pages.begin());
            if (m_Frames[it->m_Address].m_pOwner != 0) MakeFrameAvailable(it->m_Address);
        } else {
            // Check if the page is already in memory
            if (!it->m_Present) {
                // Allocate a frame for the page
                it->m_Address = GetAvailableFrame();
            } else {
                // Look for the frame it uses in the recently accessed ones
                FrameIndexDeque::iterator frame = std::find(m_Recent.begin(), m_Recent.end(), it->m_Address);

                // Check if we found it there
                if (frame != m_Recent.end()) {
                    // Remove it from the deque
                    m_Recent.erase(frame);
                } else {
                    // The frame is bound to be in the other deque, so remove it
                    frame = std::find(m_NotRecent.begin(), m_NotRecent.end(), it->m_Address);
                    assert(frame != m_NotRecent.end());
                    m_NotRecent.erase(frame);
                }
            }
        }

        // Compute the table and page index within the mapable
        size_t table = (it - p_spMapable->m_Pages.begin()) / PAGE_TABLE_CAPACITY;
        size_t page  = (it - p_spMapable->m_Pages.begin()) % PAGE_TABLE_CAPACITY;
            
        // Map the page to the frame we previously selected
        Machine::MapPageToFrame(p_spMapable->m_Tables[table], page, it->m_Address);
        m_Frames[it->m_Address].m_pOwner = p_spMapable.get();
        m_Frames[it->m_Address].m_Index  = it - p_spMapable->m_Pages.begin();

        // TODO: Initialize the page if this hasn't been done yet!

        // Update the page structure
        it->m_Initialized   = true;
        it->m_Present       = true;
    }

    // Mark the mapable as locked
    p_spMapable->m_Locked = true;
}

//******************************************************************************
// Unlocks a previously locked mapable from memory.
//
// Parameters:
//  p_spMapable - The mapable to lock into memory.
//******************************************************************************
void Pager::UnlockMapable(MapableSP p_spMapable)
{
    assert(this != 0);
    assert(p_spMapable != 0);
    Threading::InterruptLock intlock;
    Threading::SpinLockLocker lock1(m_SpinLock);
    Threading::SpinLockLocker lock2(p_spMapable->m_SpinLock);
    assert(p_spMapable->m_Locked);

    // Go  through all  the  pages of the mapable and  make the frame they use
    // available for being used by other pages.
    for (Mapable::PageVector::iterator it = p_spMapable->m_Pages.begin(); it != p_spMapable->m_Pages.end(); ++it) {
        // Put the current frame in the recently accessed deque
        m_Recent.push_back(it->m_Address);
    }

    // Mark the mapable as unlocked
    p_spMapable->m_Locked = false;
}

//******************************************************************************
// Handler for page faults.
//
// Parameters:
//  p_Adress - The linear address that caused the fault.
//******************************************************************************
void Pager::PageFault(size_t p_Address)
{
    assert(this != 0);
    Threading::InterruptLock intlock;
    Threading::SpinLockLocker lock(m_SpinLock);

    PANIC("PAGE FAULT!");

    // Retrieve a pointer to the pageable that caused the fault
    Pageable* pPageable = g_pScheduler->Current()->OwnerProcess();

    // Compute the address of the beginning of the faulting page
    size_t address = p_Address - p_Address % OS_PAGE_SIZE;

    // Attempt to retrieve an iterator to the mapable that should  contain the
    // faulting page (if such a mapable exists).
    Pageable::MapableMap::iterator mapable = pPageable->m_Mapables.upper_bound(address);

    // Ensure the page fault is 'valid' (within a mapable we know about...)
    if (mapable == pPageable->m_Mapables.end() || address < mapable->first - mapable->second->Size()) {
        // TODO: Handle faulty page faults !
        PANIC("Page fault outside any valid mapable!");
    }

    // Compute the index  of the page within the mapable, and the index of the
    // page table and the page within the same mapable.
    size_t index = (address - mapable->first + mapable->second->Size()) / OS_PAGE_SIZE;
    size_t table = index / PAGE_TABLE_CAPACITY;
    size_t page  = index % PAGE_TABLE_CAPACITY;

    // Retrieve an available frame, and  put  it back in the recently accessed
    // queue so that it will be a candidate for future replacement.
    size_t frame = GetAvailableFrame();
    m_Recent.push_back(frame);

    // Map the page to the frame we got
    Machine::MapPageToFrame(mapable->second->m_Tables[table], page, frame);

    // Retrieve a pointer on the page structure of the page we just mapped
    Mapable::Page* pPage = &mapable->second->m_Pages[index];

    // Check if the page has already been initialized
    if (pPage->m_Initialized) {
        // TODO: Load the page from the page file!
    } else {
        // Zero the memory of the page and mark it as initialized
        memset(reinterpret_cast<void*>(address), 0, OS_PAGE_SIZE);
        pPage->m_Initialized = true;
    }

    // Update frame information
    m_Frames[frame].m_pOwner = mapable->second.get();
    m_Frames[frame].m_Index  = index;

    // Update page information
    pPage->m_Present = true;
    pPage->m_Address = frame;
}

//******************************************************************************
// Returns the size of the kernel memory.
//******************************************************************************
size_t Pager::KernelSize() const
{
    assert(this != 0);
    Threading::InterruptLock intlock;
    Threading::SpinLockLocker lock(m_KernelSpinLock);

    return m_KernelSize;
}

//******************************************************************************
// Changes the size of the kernel memory.
//
// Parameters:
//  p_Size - The new size of the kernel memory, in bytes.
//******************************************************************************
void Pager::KernelSize(size_t p_Size)
{
    assert(this != 0);
    assert(p_Size >= &_END_OF_BSS - &_BEGINNING_OF_TEXT);
    assert(p_Size % OS_PAGE_SIZE == 0);
    Threading::InterruptLock intlock;
    Threading::SpinLockLocker lock(m_KernelSpinLock);

    // Convert the specified size to page units instead of bytes
    p_Size /= OS_PAGE_SIZE;

    // Check if we're growing or reducing the kernel memory size
    if (p_Size > m_KernelSize) {
        // Map enough new page tables to hold the new memory
        for (size_t i = m_KernelSize; i < p_Size; i = Utilities::RoundUp(i + 1, PAGE_TABLE_CAPACITY)) {
            // Get the descriptor of the current page table
            size_t table = m_KernelTables[i / PAGE_TABLE_CAPACITY];

            // Compute the index of the last page that we'll have to map
            size_t end = p_Size - i > PAGE_TABLE_CAPACITY ? PAGE_TABLE_CAPACITY : p_Size % PAGE_TABLE_CAPACITY;

            // Map pages in the current table to a frame in physical memory
            for (size_t j = i % PAGE_TABLE_CAPACITY; j < end; ++j) {
                // We must take the frame among  the ones available for kernel
                // memory  use,  because  using  the  usual  way  of obtaining
                // frames could result in  memory allocations (while writing a
                // dirty page, for example) that  would cause a reentrant call
                // to malloc, which probably is who's calling us here...

                // If the available frame deque is empty we're in deep shit
                if (m_KernelFrames.empty()) {
                    // TODO: (we *could* attempt to  get some free frames from
                    // the other frame deques, without reallocation...)
                    PANIC("Out of available frames for kernel memory!");
                }

                // Retrieve an available frame and map the page to it
                Machine::MapPageToFrame(table, j, m_KernelFrames.back());
                m_KernelFrames.pop_back();
            }

            // Go through all pageables and map the current page table
            for (PageableVector::iterator it = m_Pageables.begin(); it != m_Pageables.end(); ++it) {
                // Map the page table within the current pageable
                Machine::MapPageTableToDirectory((*it)->m_Directory, table, KERNEL_SPACE_BOUNDARY / PAGE_TABLE_SIZE + i / PAGE_TABLE_CAPACITY);
            }

            // TODO: Maybe zero the allocated memory?
        }
    } else {
        // TODO: Implement!
        assert(false || p_Size == m_KernelSize);
    }

    // Update the kernel memory size
    m_KernelSize = p_Size;
}

//******************************************************************************
// Prepares for the  next kernel  resizing  operation. This  is supposed to be
// called  after memory allocations to ensure that a minimum number  of frames
// are available for future kernel memory uses.
//******************************************************************************
void Pager::PrepareNextKernelSize()
{
    assert(this != 0);
    Threading::InterruptLock intlock;
    Threading::SpinLockLocker lock(m_SpinLock);
    Threading::SpinLockLocker kernelLock(m_KernelSpinLock);

    // If  we  already  had  the  spinlock  it  means  that we're being called
    // following a memory allocation within the pager, so just bail out.
    if (m_SpinLock.Count() > 1) return;

    // Check if the kernel frame deque is smaller or bigger than necessary
    if (m_KernelFrames.size() < KERNEL_FRAME_COUNT) {
        // Loop until it's size becomes large enough
        while (m_KernelFrames.size() < KERNEL_FRAME_COUNT) {
            // This will be the frame we'll retrieve
            size_t frame;

            // Retrieve  an available frame.  We  must  unlock the kernel lock
            // while doing so in order for memory allocations to be safe.
            {
                Threading::SpinLockUnlocker kernelUnlock(m_KernelSpinLock);
                frame = GetAvailableFrame();
            }

            // Now  add  the frame to the  ones  available for  the kernel. It
            // won't trigger an allocation since it is  guaranteed that  it is
            // already big enough.
            m_KernelFrames.push_back(frame);
        }
    } else {
        // Loop until it's size is small enough
        while (m_KernelFrames.size() > KERNEL_FRAME_COUNT) {
            // Retrieve an available frame
            size_t frame = m_KernelFrames.back();
            m_KernelFrames.pop_back();

            // Add it to the deque of not recently used frames
            {
                Threading::SpinLockUnlocker kernelUnlock(m_KernelSpinLock);
                m_NotRecent.push_front(frame);
            }
        }
        
    }
}

//******************************************************************************
// Returns an available frame.
//******************************************************************************
size_t Pager::GetAvailableFrame()
{
    assert(this != 0);

    // TODO: If we  have no  more frame  at all, it means  the kernel uses all
    // physical memory, and that we are in a really bad situation!
    assert(!(m_Recent.empty() && m_NotRecent.empty()));

    // Loop until a suitable frame remains in /m_NotRecent/
    do {
        // Ensure there is some frames in the not recently accessed deque
        while (m_Recent.size() > m_NotRecent.size()) {
            // Retrieve a pointer to the current frame
            Frame* pFrame = &m_Frames[m_Recent.front()];

            // Reset the current frame's accessed flag
            Machine::ResetPageAccessedFlag(pFrame->m_pOwner->m_Tables[pFrame->m_Index / PAGE_TABLE_CAPACITY], pFrame->m_Index % PAGE_TABLE_CAPACITY);

            // Transfer the frame to the other deque
            m_NotRecent.push_back(m_Recent.front());
            m_Recent.pop_front();
        }

        // Transfer  back  all frames  that  were accessed at the front of
        // the not recently accessed deque to the recently accessed one.
        while (!m_NotRecent.empty()) {
            // Retrieve a  pointer to  the current frame
            Frame* pFrame = &m_Frames[m_NotRecent.front()];

            // Stop if we encounter a free frame
            if (pFrame->m_pOwner == 0) break;
            
            // Stop if we encounter a non-accessed frame
            if (Machine::ResetPageAccessedFlag(pFrame->m_pOwner->m_Tables[pFrame->m_Index / PAGE_TABLE_CAPACITY], pFrame->m_Index % PAGE_TABLE_CAPACITY)) break;

            // Transfer the frame to the other deque
            m_Recent.push_back(m_NotRecent.front());
            m_NotRecent.pop_front();
        }
    } while (m_NotRecent.empty());

    // Retrieve the frame from the deque
    size_t frame = m_NotRecent.front();
    m_NotRecent.pop_front();

    // Ensure the frame is available
    if (m_Frames[frame].m_pOwner != 0) MakeFrameAvailable(frame);

    return frame;
}

//******************************************************************************
// Ensures a frame is available.
//
// Parameters:
//  p_Frame - The frame to make available.
//******************************************************************************
void Pager::MakeFrameAvailable(size_t p_Frame)
{
    assert(this != 0);
    assert(m_Frames[p_Frame].m_pOwner != 0);

    assert(false);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Pager::Mapable class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//
// Parameters:
//  p_Size   - The size of the mapable, in bytes.
//******************************************************************************
Mapable::Mapable(size_t p_Size)
:   m_Tables(),
    m_Pages(),
    m_Locked(false)
{
    assert(this != 0);
    assert(p_Size != 0);
    assert(p_Size % OS_PAGE_SIZE == 0);

    // Allocate enough page tables to accomodate the requested size
    for (size_t i = 0; i * PAGE_TABLE_SIZE < p_Size; ++i) {
        // Allocate a new page table
        m_Tables.push_back(Machine::AllocatePageTableDescriptor());
    }

    // Grow the page vector to the specified size
    m_Pages.resize(p_Size / OS_PAGE_SIZE);

    // Initialize all pages within the vector
    for (PageVector::iterator it = m_Pages.begin(); it != m_Pages.end(); ++it) {
        // Initialize the current page
        it->m_Initialized = false;
        it->m_Present     = false;
        it->m_Address     = 0;
    }
}

//******************************************************************************
// Destructor.
//******************************************************************************
Mapable::~Mapable()
{
    assert(this != 0);
}

//******************************************************************************
// Returns the size of the mapable, in bytes.
//******************************************************************************
size_t Mapable::Size() const
{
    assert(this != 0);

    return m_Pages.size() * OS_PAGE_SIZE;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Pager::Pageable class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//******************************************************************************
Pageable::Pageable()
:   m_Directory(),
    m_Blocks(),
    m_Mapables()
{
    assert(this != 0);

    // Allocate a page directory for the pageable
    m_Directory = Machine::AllocatePageDirectoryDescriptor();

    // Initialize the free blocks to all the user address space
    m_Blocks.Deallocate(0, KERNEL_SPACE_BOUNDARY);
}

//******************************************************************************
// Destructor.
//******************************************************************************
Pageable::~Pageable()
{
    assert(this != 0);

    // Release our page directory
    Machine::ReleasePageDirectoryDescriptor(m_Directory);
}

//******************************************************************************
// Maps a mapable within the pageable.
//
// Parameters:
//  p_spMapable - The mapable to map within the pageable.
//  p_Address   - The virtual address at which to map the mapable.
//
// Returns:
//  The address at which the mapable was mapped.
//******************************************************************************
size_t Pageable::Map(MapableSP p_spMapable, size_t p_Address)
{
    assert(this != 0);
    assert(p_spMapable != 0);
    assert(p_Address % PAGE_TABLE_SIZE == 0 || p_Address == 0xFFFFFFFF);
    Threading::InterruptLock intlock;
    Threading::SpinLockLocker lock1(m_SpinLock);
    Threading::SpinLockLocker lock2(p_spMapable->m_SpinLock);

    // Allocate space for the mapable within the address space if needed
    if (p_Address == 0xFFFFFFFF) {
        p_Address = m_Blocks.Allocate(Utilities::RoundUp(p_spMapable->Size(), PAGE_TABLE_SIZE));
    } else {
        // Force the allocation of the specified memory
        m_Blocks.ForceAllocate(p_Address, Utilities::RoundUp(p_spMapable->Size(), PAGE_TABLE_SIZE));
    }

    // Add the mapable to the map
    m_Mapables.insert(std::make_pair(p_Address + p_spMapable->Size(), p_spMapable));

    // Map all the page tables of the mapable into our directory
    for (Mapable::TableVector::iterator it = p_spMapable->m_Tables.begin(); it != p_spMapable->m_Tables.end(); ++it) {
        // Map the current page table to the pageable's page directory
        Machine::MapPageTableToDirectory(m_Directory, *it, p_Address / PAGE_TABLE_SIZE + (it - p_spMapable->m_Tables.begin()));
    }

    return p_Address;
}

//******************************************************************************
// Unmaps a mapable from the pageable.
//
// Parameters:
//  p_spMapable - The mapable to unmap from the pageable.
//******************************************************************************
void Pageable::Unmap(MapableSP p_spMapable)
{
    assert(this != 0);
    assert(p_spMapable != 0);
    Threading::InterruptLock intlock;
    Threading::SpinLockLocker lock1(m_SpinLock);
    Threading::SpinLockLocker lock2(p_spMapable->m_SpinLock);

    // TODO: Implement!
    assert(false);
}

} // namespace Paging
} // namespace Nutshell
