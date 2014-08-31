//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/TaskStateSegment.h"

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// Constructor.
//******************************************************************************
TaskStateSegment::TaskStateSegment()
{
    assert(this != 0);
    assert(reinterpret_cast<unsigned>(this) % 8 == 0);

    // Fill the task state segment with 0's
    memset(m_Data, 0, sizeof(m_Data));
}

//******************************************************************************
// Destructor.
//******************************************************************************
TaskStateSegment::~TaskStateSegment()
{
    assert(this != 0);
}

//******************************************************************************
// Gets the value of the PreviousTaskLink field.
//******************************************************************************
unsigned short TaskStateSegment::PreviousTaskLink() const
{
    assert(this != 0);

    return m_Data[0] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the PreviousTaskLink field.
//******************************************************************************
void TaskStateSegment::PreviousTaskLink(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[0] &= ~0xFFFF;
    m_Data[0] |= p_Value;
}

//******************************************************************************
// Gets the value of the ESP0 field.
//******************************************************************************
unsigned TaskStateSegment::ESP0() const
{
    assert(this != 0);

    return m_Data[1];
}

//******************************************************************************
// Sets the value of the ESP0 field.
//******************************************************************************
void TaskStateSegment::ESP0(unsigned p_Value)
{
    assert(this != 0);

    m_Data[1] = p_Value;
}

//******************************************************************************
// Gets the value of the SS0 field.
//******************************************************************************
unsigned short TaskStateSegment::SS0() const
{
    assert(this != 0);

    return m_Data[2] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the SS0 field.
//******************************************************************************
void TaskStateSegment::SS0(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[2] &= ~0xFFFF;
    m_Data[2] |= p_Value;
}

//******************************************************************************
// Gets the value of the ESP1 field.
//******************************************************************************
unsigned TaskStateSegment::ESP1() const
{
    assert(this != 0);

    return m_Data[3];
}

//******************************************************************************
// Sets the value of the ESP1 field.
//******************************************************************************
void TaskStateSegment::ESP1(unsigned p_Value)
{
    assert(this != 0);

    m_Data[3] = p_Value;
}

//******************************************************************************
// Gets the value of the SS1 field.
//******************************************************************************
unsigned short TaskStateSegment::SS1() const
{
    assert(this != 0);

    return m_Data[4] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the SS1 field.
//******************************************************************************
void TaskStateSegment::SS1(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[4] &= ~0xFFFF;
    m_Data[4] |= p_Value;
}

//******************************************************************************
// Gets the value of the ESP2 field.
//******************************************************************************
unsigned TaskStateSegment::ESP2() const
{
    assert(this != 0);

    return m_Data[5];
}

//******************************************************************************
// Sets the value of the ESP2 field.
//******************************************************************************
void TaskStateSegment::ESP2(unsigned p_Value)
{
    assert(this != 0);

    m_Data[5] = p_Value;
}

//******************************************************************************
// Gets the value of the SS2 field.
//******************************************************************************
unsigned short TaskStateSegment::SS2() const
{
    assert(this != 0);

    return m_Data[6] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the SS2 field.
//******************************************************************************
void TaskStateSegment::SS2(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[6] &= ~0xFFFF;
    m_Data[6] |= p_Value;
}

//******************************************************************************
// Gets the value of the CR3 field.
//******************************************************************************
unsigned TaskStateSegment::CR3() const
{
    assert(this != 0);

    return m_Data[7];
}

//******************************************************************************
// Sets the value of the CR3 field.
//******************************************************************************
void TaskStateSegment::CR3(unsigned p_Value)
{
    assert(this != 0);

    m_Data[7] = p_Value;
}

//******************************************************************************
// Gets the value of the EIP field.
//******************************************************************************
unsigned TaskStateSegment::EIP() const
{
    assert(this != 0);

    return m_Data[8];
}

//******************************************************************************
// Sets the value of the EIP field.
//******************************************************************************
void TaskStateSegment::EIP(unsigned p_Value)
{
    assert(this != 0);

    m_Data[8] = p_Value;
}

//******************************************************************************
// Gets the value of the EFLAGS field.
//******************************************************************************
unsigned TaskStateSegment::EFLAGS() const
{
    assert(this != 0);

    return m_Data[9];
}

//******************************************************************************
// Sets the value of the EFLAGS field.
//******************************************************************************
void TaskStateSegment::EFLAGS(unsigned p_Value)
{
    assert(this != 0);

    m_Data[9] = p_Value;
}

//******************************************************************************
// Gets the value of the EAX field.
//******************************************************************************
unsigned TaskStateSegment::EAX() const
{
    assert(this != 0);

    return m_Data[10];
}

//******************************************************************************
// Sets the value of the EAX field.
//******************************************************************************
void TaskStateSegment::EAX(unsigned p_Value)
{
    assert(this != 0);

    m_Data[10] = p_Value;
}

//******************************************************************************
// Gets the value of the ECX field.
//******************************************************************************
unsigned TaskStateSegment::ECX() const
{
    assert(this != 0);

    return m_Data[11];
}

//******************************************************************************
// Sets the value of the ECX field.
//******************************************************************************
void TaskStateSegment::ECX(unsigned p_Value)
{
    assert(this != 0);

    m_Data[11] = p_Value;
}

//******************************************************************************
// Gets the value of the EDX field.
//******************************************************************************
unsigned TaskStateSegment::EDX() const
{
    assert(this != 0);

    return m_Data[12];
}

//******************************************************************************
// Sets the value of the EDX field.
//******************************************************************************
void TaskStateSegment::EDX(unsigned p_Value)
{
    assert(this != 0);

    m_Data[12] = p_Value;
}

//******************************************************************************
// Gets the value of the EBX field.
//******************************************************************************
unsigned TaskStateSegment::EBX() const
{
    assert(this != 0);

    return m_Data[13];
}

//******************************************************************************
// Sets the value of the EBX field.
//******************************************************************************
void TaskStateSegment::EBX(unsigned p_Value)
{
    assert(this != 0);

    m_Data[13] = p_Value;
}

//******************************************************************************
// Gets the value of the ESP field.
//******************************************************************************
unsigned TaskStateSegment::ESP() const
{
    assert(this != 0);

    return m_Data[14];
}

//******************************************************************************
// Sets the value of the ESP field.
//******************************************************************************
void TaskStateSegment::ESP(unsigned p_Value)
{
    assert(this != 0);

    m_Data[14] = p_Value;
}

//******************************************************************************
// Gets the value of the EBP field.
//******************************************************************************
unsigned TaskStateSegment::EBP() const
{
    assert(this != 0);

    return m_Data[15];
}

//******************************************************************************
// Sets the value of the EBP field.
//******************************************************************************
void TaskStateSegment::EBP(unsigned p_Value)
{
    assert(this != 0);

    m_Data[15] = p_Value;
}

//******************************************************************************
// Gets the value of the ESI field.
//******************************************************************************
unsigned TaskStateSegment::ESI() const
{
    assert(this != 0);

    return m_Data[16];
}

//******************************************************************************
// Sets the value of the ESI field.
//******************************************************************************
void TaskStateSegment::ESI(unsigned p_Value)
{
    assert(this != 0);

    m_Data[16] = p_Value;
}

//******************************************************************************
// Gets the value of the EDI field.
//******************************************************************************
unsigned TaskStateSegment::EDI() const
{
    assert(this != 0);

    return m_Data[17];
}

//******************************************************************************
// Sets the value of the EDI field.
//******************************************************************************
void TaskStateSegment::EDI(unsigned p_Value)
{
    assert(this != 0);

    m_Data[17] = p_Value;
}

//******************************************************************************
// Gets the value of the ES field.
//******************************************************************************
unsigned short TaskStateSegment::ES() const
{
    assert(this != 0);

    return m_Data[18] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the ES field.
//******************************************************************************
void TaskStateSegment::ES(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[18] &= ~0xFFFF;
    m_Data[18] |= p_Value;
}

//******************************************************************************
// Gets the value of the CS field.
//******************************************************************************
unsigned short TaskStateSegment::CS() const
{
    assert(this != 0);

    return m_Data[19] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the CS field.
//******************************************************************************
void TaskStateSegment::CS(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[19] &= ~0xFFFF;
    m_Data[19] |= p_Value;
}

//******************************************************************************
// Gets the value of the SS field.
//******************************************************************************
unsigned short TaskStateSegment::SS() const
{
    assert(this != 0);

    return m_Data[20] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the SS field.
//******************************************************************************
void TaskStateSegment::SS(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[20] &= ~0xFFFF;
    m_Data[20] |= p_Value;
}

//******************************************************************************
// Gets the value of the DS field.
//******************************************************************************
unsigned short TaskStateSegment::DS() const
{
    assert(this != 0);

    return m_Data[21] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the DS field.
//******************************************************************************
void TaskStateSegment::DS(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[21] &= ~0xFFFF;
    m_Data[21] |= p_Value;
}

//******************************************************************************
// Gets the value of the FS field.
//******************************************************************************
unsigned short TaskStateSegment::FS() const
{
    assert(this != 0);

    return m_Data[22] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the FS field.
//******************************************************************************
void TaskStateSegment::FS(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[22] &= ~0xFFFF;
    m_Data[22] |= p_Value;
}

//******************************************************************************
// Gets the value of the GS field.
//******************************************************************************
unsigned short TaskStateSegment::GS() const
{
    assert(this != 0);

    return m_Data[23] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the GS field.
//******************************************************************************
void TaskStateSegment::GS(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[23] &= ~0xFFFF;
    m_Data[23] |= p_Value;
}

//******************************************************************************
// Gets the value of the LDTSegmentSelector field.
//******************************************************************************
unsigned short TaskStateSegment::LDTSegmentSelector() const
{
    assert(this != 0);

    return m_Data[24] & 0xFFFF;
}

//******************************************************************************
// Sets the value of the LDTSegmentSelector field.
//******************************************************************************
void TaskStateSegment::LDTSegmentSelector(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[24] &= ~0xFFFF;
    m_Data[24] |= p_Value;
}

//******************************************************************************
// Gets the value of the IOMapBaseAddress field.
//******************************************************************************
unsigned short TaskStateSegment::IOMapBaseAddress() const
{
    assert(this != 0);

    return (m_Data[25] & 0xFFFF0000) >> 16;
}

//******************************************************************************
// Sets the value of the IOMapBaseAddress field.
//******************************************************************************
void TaskStateSegment::IOMapBaseAddress(unsigned short p_Value)
{
    assert(this != 0);

    m_Data[25] &= ~0xFFFF0000;
    m_Data[25] |= p_Value << 16;
}

//******************************************************************************
// Gets the value of the T field.
//******************************************************************************
bool TaskStateSegment::T() const
{
    assert(this != 0);

    return m_Data[25] & 1;
}

//******************************************************************************
// Sets the value of the T field.
//******************************************************************************
void TaskStateSegment::T(bool p_Value)
{
    assert(this != 0);

    m_Data[25] &= ~1;
    if (p_Value) m_Data[25] |= 1;
}

} // namespace Intel386
} // namespace Nutshell
