//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/InterruptDescriptorTable.h"

namespace Nutshell {
namespace Intel386 {

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// GateDescriptor class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//******************************************************************************
GateDescriptor::GateDescriptor()
:   m_Data1(0),
    m_Data2(0)
{
    assert(this != 0);
    assert(reinterpret_cast<unsigned>(this) % 8 == 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
GateDescriptor::~GateDescriptor()
{
    assert(this != 0);
}

//******************************************************************************
// Gets the value of the segment field.
//******************************************************************************
unsigned short GateDescriptor::Segment() const
{
    assert(this != 0);

    return (m_Data1 & 0xFFFF0000) >> 16; 
}

//******************************************************************************
// Sets the value of the segment field.
//******************************************************************************
void GateDescriptor::Segment(unsigned short p_Value)
{
    assert(this != 0);

    m_Data1 &= ~0xFFFF0000;
    m_Data1 |= p_Value << 16;
}
//******************************************************************************
// Gets the value of the offset field.
//******************************************************************************
unsigned GateDescriptor::Offset() const
{
    assert(this != 0);

    return (m_Data1 & 0xFFFF) | (m_Data2 & 0xFFFF0000); 
}

//******************************************************************************
// Sets the value of the offset field.
//******************************************************************************
void GateDescriptor::Offset(unsigned p_Value)
{
    assert(this != 0);

    m_Data1 &= ~0xFFFF;
    m_Data1 |= p_Value & 0xFFFF;
    m_Data2 &= ~0xFFFF0000;
    m_Data2 |= p_Value & 0xFFFF0000;
}

//******************************************************************************
// Gets the value of the P field.
//******************************************************************************
bool GateDescriptor::P() const
{
    assert(this != 0);

    return m_Data2 & 1 << 15;
}

//******************************************************************************
// Sets the value of the P field.
//******************************************************************************
void GateDescriptor::P(bool p_Value)
{
    assert(this != 0);

    m_Data2 &= ~(1 << 15);
    if (p_Value) m_Data2 |= (1 << 15);
}

//******************************************************************************
// Gets the value of the DPL field.
//******************************************************************************
int GateDescriptor::DPL() const
{
    assert(this != 0);

    return (m_Data2 & 0x6000) >> 13;
}

//******************************************************************************
// Sets the value of the DPL field.
//******************************************************************************
void GateDescriptor::DPL(int p_Value)
{
    assert(this != 0);
    assert(p_Value >= 0);
    assert(p_Value <= 3);

    m_Data2 &= ~(0x6000);
    m_Data2 |= p_Value << 13;
}

//******************************************************************************
// Gets the value of the D field.
//******************************************************************************
bool GateDescriptor::D() const
{
    assert(this != 0);

    return m_Data2 & 1 << 11;
}

//******************************************************************************
// Sets the value of the D field.
//******************************************************************************
void GateDescriptor::D(bool p_Value)
{
    assert(this != 0);

    m_Data2 &= ~(1 << 11);
    if (p_Value) m_Data2 |= (1 << 11);
}

//******************************************************************************
// Gets the value of the type field.
//******************************************************************************
GateDescriptor::Types GateDescriptor::Type() const
{
    assert(this != 0);

    return static_cast<Types>((m_Data2 & 0xE00) >> 8);
}

//******************************************************************************
// Sets the value of the type field.
//******************************************************************************
void GateDescriptor::Type(Types p_Value)
{
    assert(this != 0);
    assert(p_Value >= TaskGate);
    assert(p_Value <= TrapGate);

    m_Data2 &= ~0x700;
    m_Data2 |= p_Value << 8;
}

//******************************************************************************
// Resets the descriptor to 0.
//******************************************************************************
void GateDescriptor::Reset()
{
    assert(this != 0);

    m_Data1 = 0;
    m_Data2 = 0;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// InterruptDescriptorTable class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Constructor.
//******************************************************************************
InterruptDescriptorTable::InterruptDescriptorTable()
{
    assert(this != 0);
    assert(reinterpret_cast<unsigned>(this) % 8 == 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
InterruptDescriptorTable::~InterruptDescriptorTable()
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
const GateDescriptor& InterruptDescriptorTable::operator[](int p_Index) const
{
    assert(this != 0);
    assert(p_Index >= 0);
    assert(p_Index < 256);

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
GateDescriptor& InterruptDescriptorTable::operator[](int p_Index)
{
    assert(this != 0);
    assert(p_Index >= 0);
    assert(p_Index < 256);

    return m_Descriptors[p_Index];
}

} // namespace Intel386
} // namespace Nutshell
