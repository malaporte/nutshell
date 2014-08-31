//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_PAGEDIRECTORY_H
#define INTEL386_PAGEDIRECTORY_H

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// This class encapsulates a processor page table entry.
//******************************************************************************
class PageTableEntry {
private:

    unsigned    m_Data;     // The binary representation of the entry.

public:

    // Construction / destruction
    PageTableEntry();
    ~PageTableEntry();

    // Entry management
    unsigned    Base() const;
    void        Base(unsigned p_Address);
    bool        G() const;
    void        G(bool p_Value);
    bool        D() const;
    void        D(bool p_Value);
    bool        A() const;
    void        A(bool p_Value);
    bool        PCD() const;
    void        PCD(bool p_Value);
    bool        PWT() const;
    void        PWT(bool p_Value);
    bool        US() const;
    void        US(bool p_Value);
    bool        RW() const;
    void        RW(bool p_Value);
    bool        P() const;
    void        P(bool p_Value);
    void        Reset();
};

//******************************************************************************
// This class encapsulates a processor page table.
//******************************************************************************
class PageTable {
private:

    PageTableEntry  m_Entries[1024];    // The page table entries.

public:

    // Construction / destruction
    PageTable();
    ~PageTable();

    // Operators
    const PageTableEntry&   operator[](int p_Index) const;
    PageTableEntry&         operator[](int p_Index);
};

//******************************************************************************
// This class encapsulates a processor page directory entry.
//******************************************************************************
class PageDirectoryEntry {
private:

    unsigned    m_Data;     // The binary representation of the entry.

public:

    // Construction / destruction
    PageDirectoryEntry();
    ~PageDirectoryEntry();

    // Entry management
    unsigned    Base() const;
    void        Base(unsigned p_Address);
    bool        G() const;
    void        G(bool p_Value);
    bool        PS() const;
    void        PS(bool p_Value);
    bool        D() const;
    void        D(bool p_Value);
    bool        A() const;
    void        A(bool p_Value);
    bool        PCD() const;
    void        PCD(bool p_Value);
    bool        PWT() const;
    void        PWT(bool p_Value);
    bool        US() const;
    void        US(bool p_Value);
    bool        RW() const;
    void        RW(bool p_Value);
    bool        P() const;
    void        P(bool p_Value);
    void        Reset();
};

//******************************************************************************
// This class encapsulates a processor page directory.
//******************************************************************************
class PageDirectory {
private:

    PageDirectoryEntry  m_Entries[1024];    // The page directory entries.

public:

    // Construction / destruction
    PageDirectory();
    ~PageDirectory();

    // Operators
    const PageDirectoryEntry&   operator[](int p_Index) const;
    PageDirectoryEntry&         operator[](int p_Index);
};

typedef boost::shared_ptr<PageDirectory> PageDirectorySP;

} // namespace Intel386
} // namespace Nutshell

#endif // !INTEL386_PAGEDIRECTORY_H
