//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_TASKSTATESEGMENT_H
#define INTEL386_TASKSTATESEGMENT_H

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// This class encapsulates a processor task state segment.
//******************************************************************************
class TaskStateSegment {
private:

    unsigned        m_Data[26];     // The TSS data.

public:

    // Construction / destruction
    TaskStateSegment();
    ~TaskStateSegment();

    // Task state segment management
    unsigned short  PreviousTaskLink() const;
    void            PreviousTaskLink(unsigned short p_Value);
    unsigned        ESP0() const;
    void            ESP0(unsigned p_Value);
    unsigned short  SS0() const;
    void            SS0(unsigned short p_Value);
    unsigned        ESP1() const;
    void            ESP1(unsigned p_Value);
    unsigned short  SS1() const;
    void            SS1(unsigned short p_Value);
    unsigned        ESP2() const;
    void            ESP2(unsigned p_Value);
    unsigned short  SS2() const;
    void            SS2(unsigned short p_Value);
    unsigned        CR3() const;
    void            CR3(unsigned p_Value);
    unsigned        EIP() const;
    void            EIP(unsigned p_Value);
    unsigned        EFLAGS() const;
    void            EFLAGS(unsigned p_Value);
    unsigned        EAX() const;
    void            EAX(unsigned p_Value);
    unsigned        ECX() const;
    void            ECX(unsigned p_Value);
    unsigned        EDX() const;
    void            EDX(unsigned p_Value);
    unsigned        EBX() const;
    void            EBX(unsigned p_Value);
    unsigned        ESP() const;
    void            ESP(unsigned p_Value);
    unsigned        EBP() const;
    void            EBP(unsigned p_Value);
    unsigned        ESI() const;
    void            ESI(unsigned p_Value);
    unsigned        EDI() const;
    void            EDI(unsigned p_Value);
    unsigned short  ES() const;
    void            ES(unsigned short p_Value);
    unsigned short  CS() const;
    void            CS(unsigned short p_Value);
    unsigned short  SS() const;
    void            SS(unsigned short p_Value);
    unsigned short  DS() const;
    void            DS(unsigned short p_Value);
    unsigned short  FS() const;
    void            FS(unsigned short p_Value);
    unsigned short  GS() const;
    void            GS(unsigned short p_Value);
    unsigned short  LDTSegmentSelector() const;
    void            LDTSegmentSelector(unsigned short p_Value);
    unsigned short  IOMapBaseAddress() const;
    void            IOMapBaseAddress(unsigned short p_Value);
    bool            T() const;
    void            T(bool p_Value);
};

} // namespace Intel386
} // namespace Nutshell

#endif // !INTEL386_TASKSTATESEGMENT_H
