//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/GlobalDescriptorTable.h"

namespace Nutshell {
namespace Intel386 {

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// SegmentDescriptor class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//******************************************************************************
SegmentDescriptor::SegmentDescriptor()
:   m_Data1(0),
    m_Data2(0)
{
    assert(this != 0);
    assert(reinterpret_cast<unsigned>(this) % 8 == 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
SegmentDescriptor::~SegmentDescriptor()
{
    assert(this != 0);
}

//******************************************************************************
// Gets the value of the base field.
//******************************************************************************
unsigned SegmentDescriptor::Base() const
{
    assert(this != 0);

    return ((m_Data1 & 0xFFFF0000) >> 16) | ((m_Data2 & 0xFF) << 16) | (m_Data2 & 0xFF000000); 
}

//******************************************************************************
// Sets the value of the base field.
//******************************************************************************
void SegmentDescriptor::Base(unsigned p_Value)
{
    assert(this != 0);

    m_Data1 &= ~0xFFFF0000;
    m_Data1 |= p_Value << 16;
    m_Data2 &= ~0xFF0000FF;
    m_Data2 |= (p_Value >> 16) & 0xFF;
    m_Data2 |= p_Value & 0xFF000000;
}

//******************************************************************************
// Gets the value of the limit field.
//******************************************************************************
unsigned SegmentDescriptor::Limit() const
{
    assert(this != 0);

    return (m_Data1 & 0xFFFF) | (m_Data2 & 0xF0000); 
}

//******************************************************************************
// Sets the value of the limit field.
//******************************************************************************
void SegmentDescriptor::Limit(unsigned p_Value)
{
    assert(this != 0);

    m_Data1 &= ~0xFFFF;
    m_Data1 |= p_Value & 0xFFFF;
    m_Data2 &= ~0xF0000;
    m_Data2 |= p_Value & 0xF0000;
}

//******************************************************************************
// Gets the value of the G field.
//******************************************************************************
bool SegmentDescriptor::G() const
{
    assert(this != 0);

    return m_Data2 & 1 << 23;
}

//******************************************************************************
// Sets the value of the Granularity field.
//******************************************************************************
void SegmentDescriptor::G(bool p_Value)
{
    assert(this != 0);

    m_Data2 &= ~(1 << 23);
    if (p_Value) m_Data2 |= (1 << 23);
}

//******************************************************************************
// Gets the value of the DB field.
//******************************************************************************
bool SegmentDescriptor::DB() const
{
    assert(this != 0);

    return m_Data2 & 1 << 22;
}

//******************************************************************************
// Sets the value of the DB field.
//******************************************************************************
void SegmentDescriptor::DB(bool p_Value)
{
    assert(this != 0);

    m_Data2 &= ~(1 << 22);
    if (p_Value) m_Data2 |= (1 << 22);
}

//******************************************************************************
// Gets the value of the P field.
//******************************************************************************
bool SegmentDescriptor::P() const
{
    assert(this != 0);

    return m_Data2 & 1 << 15;
}

//******************************************************************************
// Sets the value of the P field.
//******************************************************************************
void SegmentDescriptor::P(bool p_Value)
{
    assert(this != 0);

    m_Data2 &= ~(1 << 15);
    if (p_Value) m_Data2 |= (1 << 15);
}

//******************************************************************************
// Gets the value of the DPL field.
//******************************************************************************
int SegmentDescriptor::DPL() const
{
    assert(this != 0);

    return (m_Data2 & 0x6000) >> 13;
}

//******************************************************************************
// Sets the value of the DPL field.
//******************************************************************************
void SegmentDescriptor::DPL(int p_Value)
{
    assert(this != 0);
    assert(p_Value >= 0);
    assert(p_Value <= 3);

    m_Data2 &= ~(0x6000);
    m_Data2 |= p_Value << 13;
}

//******************************************************************************
// Gets the value of the S field.
//******************************************************************************
bool SegmentDescriptor::S() const
{
    assert(this != 0);

    return m_Data2 & 1 << 12;
}

//******************************************************************************
// Sets the value of the S field.
//******************************************************************************
void SegmentDescriptor::S(bool p_Value)
{
    assert(this != 0);

    m_Data2 &= ~(1 << 12);
    if (p_Value) m_Data2 |= (1 << 12);
}

//******************************************************************************
// Gets the value of the type field.
//******************************************************************************
SegmentDescriptor::Types SegmentDescriptor::Type() const
{
    assert(this != 0);

    return static_cast<Types>((m_Data2 & 0xF00) >> 8);
}

//******************************************************************************
// Sets the value of the type field.
//******************************************************************************
void SegmentDescriptor::Type(Types p_Value)
{
    assert(this != 0);
    assert(p_Value >= ReadOnly);
    assert(p_Value <= ExecuteReadConforming);

    m_Data2 &= ~(0xF00);
    m_Data2 |= p_Value << 8;
}

//******************************************************************************
// Resets the descriptor to 0.
//******************************************************************************
void SegmentDescriptor::Reset()
{
    assert(this != 0);

    m_Data1 = 0;
    m_Data2 = 0;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// GlobalDescriptorTable class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//******************************************************************************
GlobalDescriptorTable::GlobalDescriptorTable()
{
    assert(this != 0);
    assert(reinterpret_cast<unsigned>(this) % 8 == 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
GlobalDescriptorTable::~GlobalDescriptorTable()
{
    assert(this != 0);
}

//******************************************************************************
// Operator[] (const).
//
// Parameters:
//  p_Index - The index of the descriptor to return.
//
// Returns:
//  The requested descriptor.
//******************************************************************************
const SegmentDescriptor& GlobalDescriptorTable::operator[](int p_Index) const
{
    assert(this != 0);
    assert(p_Index >= 0);
    assert(p_Index < 8192);

    return m_Descriptors[p_Index];
}

//******************************************************************************
// Operator[] (non-const).
//
// Parameters:
//  p_Index - The index of the descriptor to return.
//
// Returns:
//  The requested descriptor.
//******************************************************************************
SegmentDescriptor& GlobalDescriptorTable::operator[](int p_Index)
{
    assert(this != 0);
    assert(p_Index > 0);
    assert(p_Index < 8192);

    return m_Descriptors[p_Index];
}

} // namespace Intel386
} // namespace Nutshell
