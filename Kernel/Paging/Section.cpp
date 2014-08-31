//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Machine.h"
#include "Paging/Section.h"

namespace Nutshell {
namespace Paging {

//******************************************************************************
// Constructor.
//
// Parameters:
//  p_Size   - The size of the section, in bytes.
//******************************************************************************
Section::Section(size_t p_Size)
:   m_Locked(false)
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
Section::~Section()
{
    assert(this != 0);
}

//******************************************************************************
// Returns the size of the section, in bytes.
//******************************************************************************
size_t Section::Size() const
{
    assert(this != 0);

    return m_Pages.size() * OS_PAGE_SIZE;
}

//******************************************************************************
// Returns the size needed by the section in an address space.
//******************************************************************************
size_t Section::NeededSize() const
{
    assert(this != 0);

    return Utilities::RoundUp(Size(), PAGE_TABLE_SIZE);
}

//******************************************************************************
// Returns whether the section is locked in memory.
//******************************************************************************
bool Section::Locked() const
{
    assert(this != 0);

    return m_Locked;
}

} // namespace Paging
} // namespace Nutshell
