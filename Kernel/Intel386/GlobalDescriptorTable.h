//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_GLOBALDESCRIPTORTABLE_H
#define INTEL386_GLOBALDESCRIPTORTABLE_H

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// This class encapsulates a processor segment descriptor.
//******************************************************************************
class SegmentDescriptor {
private:

    unsigned    m_Data1;    // The first 4 bytes of data.
    unsigned    m_Data2;    // The second 4 bytes of data.

public:

    // The various segment types
    enum Types {
        // User segment types
        ReadOnly                        = 0,
        ReadOnlyAccessed                = 1,
        ReadWrite                       = 2,
        ReadWriteAccessed               = 3,
        ReadOnlyExpandDown              = 4,
        ReadOnlyExpandDownAccessed      = 5,
        ReadWriteExpandDown             = 6,
        ReadWriteExpandDownAccessed     = 7,
        ExecuteOnly                     = 8,
        ExecuteOnlyAccessed             = 9,
        ExecuteRead                     = 10,
        ExecuteReadAccessed             = 11,
        ExecuteConforming               = 12,
        ExecuteConformingAccessed       = 13,
        ExecuteReadConforming           = 14,
        ExecuteReadConformingAccessed   = 15,

        // System segment types
        TSS16BitAvailable               = 1,
        LDT                             = 2,
        TSS16BitBusy                    = 3,
        CallGate16Bit                   = 4,
        TaskGate                        = 5,
        InterruptGate16Bit              = 6,
        TrapGate16Bit                   = 7,
        TSS32BitAvailable               = 9,
        TSS32BitBusy                    = 11,
        CallGate32Bit                   = 12,
        InterruptGate32Bit              = 14,
        TrapGate32Bit                   = 15
    };

    // Construction / destruction
    SegmentDescriptor();
    ~SegmentDescriptor();

    // Descriptor management
    unsigned    Base() const;
    void        Base(unsigned p_Value);
    unsigned    Limit() const;
    void        Limit(unsigned p_Value);
    bool        G() const;
    void        G(bool p_Value);
    bool        DB() const;
    void        DB(bool p_Value);
    bool        P() const;
    void        P(bool p_Value);
    int         DPL() const;
    void        DPL(int p_Value);
    bool        S() const;
    void        S(bool p_Value);
    Types       Type() const;
    void        Type(Types p_Value);
    void        Reset();
};

//******************************************************************************
// This class encapsulates a processor global descriptor table.
//******************************************************************************
class GlobalDescriptorTable {
private:

    SegmentDescriptor   m_Descriptors[8192];    // The segment descriptors.

public:

    // Construction / destruction
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    // Operators
    const SegmentDescriptor& operator[](int p_Index) const;
    SegmentDescriptor& operator[](int p_Index);
};

} // namespace Intel386
} // namespace Nutshell

#endif // !INTEL386_GLOBALDESCRIPTORTABLE_H
