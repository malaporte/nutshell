//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/PageDirectory.h"

namespace Nutshell {
namespace Intel386 {

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// PageTableEntry class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//******************************************************************************
PageTableEntry::PageTableEntry()
:   m_Data(0)
{
    assert(this != 0);
    assert(reinterpret_cast<unsigned>(this) % 4 == 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
PageTableEntry::~PageTableEntry()
{
    assert(this != 0);
}

//******************************************************************************
// Gets the value of the base field.
//******************************************************************************
unsigned PageTableEntry::Base() const
{
    assert(this != 0);

    return m_Data & 0xFFFFF000;
}

//******************************************************************************
// Sets the value of the base field.
//******************************************************************************
void PageTableEntry::Base(unsigned p_Address)
{
    assert(this != 0);
    assert((p_Address & 0xFFFFF000) == p_Address);

    m_Data &= 0x00000FFF;
    m_Data |= p_Address;
}

//******************************************************************************
// Gets the value of the G field.
//******************************************************************************
bool PageTableEntry::G() const
{
    assert(this != 0);

    return m_Data & 1 << 8;
}

//******************************************************************************
// Sets the value of the G field.
//******************************************************************************
void PageTableEntry::G(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 8);
    if (p_Value) m_Data |= 1 << 8;
}

//******************************************************************************
// Gets the value of the D field.
//******************************************************************************
bool PageTableEntry::D() const
{
    assert(this != 0);

    return m_Data & 1 << 6;
}

//******************************************************************************
// Sets the value of the D field.
//******************************************************************************
void PageTableEntry::D(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 6);
    if (p_Value) m_Data |= 1 << 6;
}

//******************************************************************************
// Gets the value of the A field.
//******************************************************************************
bool PageTableEntry::A() const
{
    assert(this != 0);

    return m_Data & 1 << 5;
}

//******************************************************************************
// Sets the value of the A field.
//******************************************************************************
void PageTableEntry::A(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 5);
    if (p_Value) m_Data |= 1 << 5;
}

//******************************************************************************
// Gets the value of the PCD field.
//******************************************************************************
bool PageTableEntry::PCD() const
{
    assert(this != 0);

    return m_Data & 1 << 4;
}

//******************************************************************************
// Sets the value of the PCD field.
//******************************************************************************
void PageTableEntry::PCD(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 4);
    if (p_Value) m_Data |= 1 << 4;
}

//******************************************************************************
// Gets the value of the PWT field.
//******************************************************************************
bool PageTableEntry::PWT() const
{
    assert(this != 0);

    return m_Data & 1 << 3;
}

//******************************************************************************
// Sets the value of the PWT field.
//******************************************************************************
void PageTableEntry::PWT(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 3);
    if (p_Value) m_Data |= 1 << 3;
}

//******************************************************************************
// Gets the value of the US field.
//******************************************************************************
bool PageTableEntry::US() const
{
    assert(this != 0);

    return m_Data & 1 << 2;
}

//******************************************************************************
// Sets the value of the US field.
//******************************************************************************
void PageTableEntry::US(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 2);
    if (p_Value) m_Data |= 1 << 2;
}

//******************************************************************************
// Gets the value of the RW field.
//******************************************************************************
bool PageTableEntry::RW() const
{
    assert(this != 0);

    return m_Data & 1 << 1;
}

//******************************************************************************
// Sets the value of the RW field.
//******************************************************************************
void PageTableEntry::RW(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 1);
    if (p_Value) m_Data |= 1 << 1;
}

//******************************************************************************
// Gets the value of the P field.
//******************************************************************************
bool PageTableEntry::P() const
{
    assert(this != 0);

    return m_Data & 1 << 0;
}

//******************************************************************************
// Sets the value of the P field.
//******************************************************************************
void PageTableEntry::P(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 0);
    if (p_Value) m_Data |= 1 << 0;
}

//******************************************************************************
// Resets the entry to 0.
//******************************************************************************
void PageTableEntry::Reset()
{
    assert(this != 0);

    m_Data = 0;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// PageTable class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//******************************************************************************
PageTable::PageTable()
{
    assert(this != 0);
    assert(reinterpret_cast<unsigned>(this) % 0x1000 == 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
PageTable::~PageTable()
{
    assert(this != 0);
}

//******************************************************************************
// Operator[] (const).
//
// Parameters:
//  p_Index - The index of the entry to return.
//
// Returns:
//  The requested entry.
//******************************************************************************
const PageTableEntry& PageTable::operator[](int p_Index) const
{
    assert(this != 0);
    assert(p_Index >= 0);
    assert(p_Index < 1024);

    return m_Entries[p_Index];
}

//******************************************************************************
// Operator[] (non-const).
//
// Parameters:
//  p_Index - The index of the entry to return.
//
// Returns:
//  The requested entry.
//******************************************************************************
PageTableEntry& PageTable::operator[](int p_Index)
{
    assert(this != 0);
    assert(p_Index >= 0);
    assert(p_Index < 1024);

    return m_Entries[p_Index];
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// PageDirectoryEntry class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//******************************************************************************
PageDirectoryEntry::PageDirectoryEntry()
:   m_Data(0)
{
    assert(this != 0);
    assert(reinterpret_cast<unsigned>(this) % 4 == 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
PageDirectoryEntry::~PageDirectoryEntry()
{
    assert(this != 0);
}

//******************************************************************************
// Gets the value of the base field.
//******************************************************************************
unsigned PageDirectoryEntry::Base() const
{
    assert(this != 0);

    return m_Data & 0xFFFFF000;
}

//******************************************************************************
// Sets the value of the base field.
//******************************************************************************
void PageDirectoryEntry::Base(unsigned p_Address)
{
    assert(this != 0);
    assert((p_Address & 0xFFFFF000) == p_Address);

    m_Data &= 0x00000FFF;
    m_Data |= p_Address;
}

//******************************************************************************
// Gets the value of the G field.
//******************************************************************************
bool PageDirectoryEntry::G() const
{
    assert(this != 0);

    return m_Data & 1 << 8;
}

//******************************************************************************
// Sets the value of the G field.
//******************************************************************************
void PageDirectoryEntry::G(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 8);
    if (p_Value) m_Data |= 1 << 8;
}

//******************************************************************************
// Gets the value of the PS field.
//******************************************************************************
bool PageDirectoryEntry::PS() const
{
    assert(this != 0);

    return m_Data & 1 << 7;
}

//******************************************************************************
// Sets the value of the PS field.
//******************************************************************************
void PageDirectoryEntry::PS(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 7);
    if (p_Value) m_Data |= 1 << 7;
}

//******************************************************************************
// Gets the value of the D field.
//******************************************************************************
bool PageDirectoryEntry::D() const
{
    assert(this != 0);

    return m_Data & 1 << 6;
}

//******************************************************************************
// Sets the value of the D field.
//******************************************************************************
void PageDirectoryEntry::D(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 6);
    if (p_Value) m_Data |= 1 << 6;
}

//******************************************************************************
// Gets the value of the A field.
//******************************************************************************
bool PageDirectoryEntry::A() const
{
    assert(this != 0);

    return m_Data & 1 << 5;
}

//******************************************************************************
// Sets the value of the A field.
//******************************************************************************
void PageDirectoryEntry::A(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 5);
    if (p_Value) m_Data |= 1 << 5;
}

//******************************************************************************
// Gets the value of the PCD field.
//******************************************************************************
bool PageDirectoryEntry::PCD() const
{
    assert(this != 0);

    return m_Data & 1 << 4;
}

//******************************************************************************
// Sets the value of the PCD field.
//******************************************************************************
void PageDirectoryEntry::PCD(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 4);
    if (p_Value) m_Data |= 1 << 4;
}

//******************************************************************************
// Gets the value of the PWT field.
//******************************************************************************
bool PageDirectoryEntry::PWT() const
{
    assert(this != 0);

    return m_Data & 1 << 3;
}

//******************************************************************************
// Sets the value of the PWT field.
//******************************************************************************
void PageDirectoryEntry::PWT(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 3);
    if (p_Value) m_Data |= 1 << 3;
}

//******************************************************************************
// Gets the value of the US field.
//******************************************************************************
bool PageDirectoryEntry::US() const
{
    assert(this != 0);

    return m_Data & 1 << 2;
}

//******************************************************************************
// Sets the value of the US field.
//******************************************************************************
void PageDirectoryEntry::US(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 2);
    if (p_Value) m_Data |= 1 << 2;
}

//******************************************************************************
// Gets the value of the RW field.
//******************************************************************************
bool PageDirectoryEntry::RW() const
{
    assert(this != 0);

    return m_Data & 1 << 1;
}

//******************************************************************************
// Sets the value of the RW field.
//******************************************************************************
void PageDirectoryEntry::RW(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 1);
    if (p_Value) m_Data |= 1 << 1;
}

//******************************************************************************
// Gets the value of the P field.
//******************************************************************************
bool PageDirectoryEntry::P() const
{
    assert(this != 0);

    return m_Data & 1 << 0;
}

//******************************************************************************
// Sets the value of the P field.
//******************************************************************************
void PageDirectoryEntry::P(bool p_Value)
{
    assert(this != 0);

    m_Data &= ~(1 << 0);
    if (p_Value) m_Data |= 1 << 0;
}

//******************************************************************************
// Resets the entry to 0.
//******************************************************************************
void PageDirectoryEntry::Reset()
{
    assert(this != 0);

    m_Data = 0;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// PageDirectory class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//******************************************************************************
PageDirectory::PageDirectory()
{
    assert(this != 0);
    assert(reinterpret_cast<unsigned>(this) % 0x1000 == 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
PageDirectory::~PageDirectory()
{
    assert(this != 0);
}

//******************************************************************************
// Operator[] (const).
//
// Parameters:
//  p_Index - The index of the entry to return.
//
// Returns:
//  The requested entry.
//******************************************************************************
const PageDirectoryEntry& PageDirectory::operator[](int p_Index) const
{
    assert(this != 0);
    assert(p_Index >= 0);
    assert(p_Index < 1024);

    return m_Entries[p_Index];
}

//******************************************************************************
// Operator[] (non-const).
//
// Parameters:
//  p_Index - The index of the entry to return.
//
// Returns:
//  The requested entry.
//******************************************************************************
PageDirectoryEntry& PageDirectory::operator[](int p_Index)
{
    assert(this != 0);
    assert(p_Index >= 0);
    assert(p_Index < 1024);

    return m_Entries[p_Index];
}

} // namespace Intel386
} // namespace Nutshell
