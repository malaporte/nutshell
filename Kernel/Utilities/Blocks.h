//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef UTILITIES_BLOCKS_H
#define UTILITIES_BLOCKS_H

namespace Nutshell {
namespace Utilities {

//******************************************************************************
// This class manages a list of free blocks.
//******************************************************************************
class Blocks {
private:

    typedef std::map<size_t, size_t> BlockMap;

    BlockMap    m_Blocks;   // The map that contains the blocks.

public:

    // Construction / destruction
    Blocks();
    ~Blocks();

    // Blocks managements
    size_t  Allocate(size_t p_Size);
    void    ForceAllocate(size_t p_Address, size_t p_Size);
    void    Deallocate(size_t p_Address, size_t p_Size);

};

} // namespace Utilities
} // namespace Nutshell

#endif // !UTILITIES_BLOCKS_H
