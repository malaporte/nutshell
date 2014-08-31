//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_INTERRUPTDESCRIPTORTABLE_H
#define INTEL386_INTERRUPTDESCRIPTORTABLE_H

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// This class encapsulates a processor gate descriptor.
//******************************************************************************
class GateDescriptor {
private:

    unsigned    m_Data1;    // The first 4 bytes of data.
    unsigned    m_Data2;    // The second 4 bytes of data.

public:

    // The various gate types
    enum Types {
        TaskGate                = 5,
        InterruptGate           = 6,
        TrapGate                = 7,
    };

    // Construction / destruction
    GateDescriptor();
    ~GateDescriptor();

    // Descriptor management
    unsigned short  Segment() const;
    void            Segment(unsigned short p_Value);
    unsigned        Offset() const;
    void            Offset(unsigned p_Value);
    bool            P() const;
    void            P(bool p_Value);
    int             DPL() const;
    void            DPL(int p_Value);
    bool            D() const;
    void            D(bool p_Value);
    Types           Type() const;
    void            Type(Types p_Value);
    void            Reset();
};

//******************************************************************************
// This class encapsulates a processor interrupt descriptor table.
//******************************************************************************
class InterruptDescriptorTable {
private:

    GateDescriptor      m_Descriptors[256]; // The gate descriptors.

public:

    // Construction / destruction
    InterruptDescriptorTable();
    ~InterruptDescriptorTable();

    // Operators
    const GateDescriptor& operator[](int p_Index) const;
    GateDescriptor& operator[](int p_Index);
};

} // namespace Intel386
} // namespace Nutshell

#endif // !INTEL386_INTERRUPTDESCRIPTORTABLE_H
