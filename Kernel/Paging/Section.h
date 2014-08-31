//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef PAGING_SECTION_H
#define PAGING_SECTION_H

namespace Nutshell {
namespace Paging {

//******************************************************************************
// This class encapsulates a section that can be mapped to a process.
//******************************************************************************
class Section : boost::noncopyable {
private:

    //**************************************************************************
    // This holds information about a page.
    struct Page {
        bool    m_Initialized;  // Whether the page is initialized.
        bool    m_Present;      // Whether the page is present into memory.
        size_t  m_Address;      // The location at which the page is stored.
    };

    typedef std::vector<size_t> TableVector;
    typedef std::vector<Page> PageVector;

    TableVector     m_Tables;   // Vector containing the page tables for the section.
    PageVector      m_Pages;    // Vector containing the pages structures for the section.
    bool            m_Locked;   // Whether the section is locked in memory.

    friend class Pager;

public:

    // Construction / destruction
    Section(size_t p_Size);
    ~Section();

    // Section information
    size_t  Size() const;
    size_t  NeededSize() const;
    bool    Locked() const;
};

typedef boost::shared_ptr<Section> SectionSP;

} // namespace Paging
} // namespace Nutshell

#endif // !PAGING_SECTION_H
