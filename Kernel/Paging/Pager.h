//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef PAGING_PAGER_H
#define PAGING_PAGER_H

#include "Utilities/Blocks.h"
#include "Threading/SpinLock.h"

namespace Nutshell {
namespace Paging {

//******************************************************************************
// This class represents a mapable. A  mapable is a  set of  pages that can be
// mapped into the address space of one or more pageables.
//******************************************************************************
class Mapable
{
    //**************************************************************************
    // This holds information about a page.
    struct Page
    {
        bool    m_Initialized;  // Whether the page is initialized.
        bool    m_Present;      // Whether the page is present into memory.
        size_t  m_Address;      // The location at which the page is stored.
    };

    typedef std::vector<size_t> TableVector;
    typedef std::vector<Page> PageVector;

    TableVector                 m_Tables;       // Vector containing the page tables for the mapable.
    PageVector                  m_Pages;        // Vector containing the pages structures for the mapable.
    bool                        m_Locked;       // Whether the mapable is locked in memory.
    mutable Threading::SpinLock m_SpinLock;     // The lock that protects the mapable.

    friend class Pager;
    friend class Pageable;

public:
    
    // Construction / destruction
    Mapable(size_t p_Size);
    ~Mapable();

    // Mapable information
    size_t Size() const;
};

typedef boost::shared_ptr<Mapable> MapableSP;

//******************************************************************************
// This  holds information  about a  pageable. A pageable  is an address space
// which have one or more mapables mapped within it.
//******************************************************************************
class Pageable
{
    typedef std::map<size_t, MapableSP> MapableMap;

    size_t                      m_Directory;    // The page directory for the pageable.
    Utilities::Blocks           m_Blocks;       // The object used to manage the address space.
    MapableMap                  m_Mapables;     // The mapables mapped within the pageable.
    mutable Threading::SpinLock m_SpinLock;     // The lock that protects the pageable.

    friend class Pager;

public:
    
    // Construction / destruction
    Pageable();
    ~Pageable();

    size_t Directory() const { return m_Directory; };

    // Mapable management
    size_t  Map(MapableSP p_spMapable, size_t p_Address = 0xFFFFFFFF);
    void    Unmap(MapableSP p_spMapable);
};

typedef boost::shared_ptr<Pageable> PageableSP;

//******************************************************************************
// This class encapsulates the pager.
//******************************************************************************
class Pager : boost::noncopyable
{
    // The number of frames that must be kept available for the kernel.
    static const size_t KERNEL_FRAME_COUNT = 256;

    //**************************************************************************
    // This holds information about a frame.
    struct Frame
    {
        Mapable*    m_pOwner;   // The object that currently owns the frame.
        size_t      m_Index;    // The index of the page within the mapable.
    };

    typedef std::vector<PageableSP> PageableVector;
    typedef std::vector<MapableSP>  MapableVector;
    typedef std::vector<Frame>      FrameVector;
    typedef std::vector<size_t>     FrameIndexVector;
    typedef std::deque<size_t>      FrameIndexDeque;
    typedef std::vector<size_t>     TableVector;

    PageableVector              m_Pageables;        // Vector that contains all the pageables.          
    MapableVector               m_Globals;          // Vector that contains the global mapables.

    FrameVector                 m_Frames;           // Vector that contains the frames.
    FrameIndexDeque             m_Recent;           // Deque of recently accessed frames.
    FrameIndexDeque             m_NotRecent;        // Deque of less recently accessed frames.

    mutable Threading::SpinLock m_SpinLock;         // The spin lock that protects the pager.

    size_t                      m_KernelSize;       // The total size of the kernel memory, in pages.
    TableVector                 m_KernelTables;     // Vector that contains the kernel page tables.
    FrameIndexVector            m_KernelFrames;     // Vector that contains the frames reserved for kernel use.

    mutable Threading::SpinLock m_KernelSpinLock;   // Spin lock to protect the kernel members only.

public:

    // Construction / destruction
    Pager();
    ~Pager();

    // Pageable management
    void    AddPageable(PageableSP p_spPageable);
    void    RemovePageable(PageableSP p_spPageable);

    // Mapable management
    void    LockMapable(MapableSP p_spMapable, size_t p_Address = 0xFFFFFFFF);
    void    UnlockMapable(MapableSP p_spMapable);

    // Interrupts handlers
    void    PageFault(size_t p_Address);

    // Kernel size management
    size_t  KernelSize() const;
    void    KernelSize(size_t p_Size);
    void    PrepareNextKernelSize();

private:

    // Misceallenous
    size_t  GetAvailableFrame();
    void    MakeFrameAvailable(size_t p_Frame);
};

} // namespace Paging
} // namespace Nutshell

#endif // !PAGING_PAGER_H
