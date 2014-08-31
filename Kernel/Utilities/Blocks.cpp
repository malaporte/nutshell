//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Utilities/Blocks.h"

namespace Nutshell {
namespace Utilities {

//******************************************************************************
// Constructor.
//******************************************************************************
Blocks::Blocks()
{
    assert(this != 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
Blocks::~Blocks()
{
    assert(this != 0);
}

//******************************************************************************
// Allocates a block.
//
// Parameters:
//  p_Size    - The size of the requested block (in bytes).
//
// Returns:
//  The address of the beginning of the block.
//******************************************************************************
size_t Blocks::Allocate(size_t p_Size)
{
    assert(this != 0);
    assert(p_Size != 0);

    // Go through the block list until we find one that is large enough
    for (BlockMap::iterator it = m_Blocks.begin(); it != m_Blocks.end(); ++it) {
        // Check if the current block is large enough
        if (it->second >= p_Size) {
            // Check if space will remain in the block after allocation
            if (it->second > p_Size) {
                // Create a new block that will contain the remaining space
                m_Blocks.insert(std::make_pair(it->first + p_Size, it->second - p_Size));
            }

            // Remember the address of the block and remove it from the map
            size_t address = it->first;
            m_Blocks.erase(it);

            return address;
        }
    }

    // If we get here, it means that we're out of memory.
    // TODO: Do something like throwing an exception!
    assert(false);
}

//******************************************************************************
// Force the allocation of a specific block.
//
// Parameters:
//  p_Address - The address where to allocate the block.
//  p_Size    - The size of the block (in bytes).
//******************************************************************************
void Blocks::ForceAllocate(size_t p_Address, size_t p_Size)
{
    assert(this != 0);
    assert(p_Size != 0);

    // Go through the block list and find the one that contains the address
    for (BlockMap::iterator it = m_Blocks.begin(); it != m_Blocks.end(); ++it) {
        // Check if the current block holds the address
        if (it->first <= p_Address && it->first + it->second > p_Address) {
            // The block should  be large enough, and if not it is a bug since
            // when we're force allocating something it must be free.
            assert(p_Address + p_Size < it->first + it->second);

            // If space will remain at the right, create a new block
            if (it->first + it->second > p_Address + p_Size) {
                m_Blocks.insert(std::make_pair(p_Address + p_Size, (it->first + it->second) - (p_Address + p_Size)));
            }

            // Reduce the size of the block, and if it becomes empty, remove it
            it->second = p_Address - it->first;
            if (it->second == 0) {
                m_Blocks.erase(it);
            }

            return;
        }
    }

    // If we get here, it means that the requested memory isn't available!
    assert(false);
}

//******************************************************************************
// Deallocates a block.
//
// Parameters:
//  p_Address   - The address of the beginning of the block.
//  p_Size      - The size of the block.
//******************************************************************************
void Blocks::Deallocate(size_t p_Address, size_t p_Size)
{
    assert(this != 0);
    assert(p_Size != 0);

    // Insert the new block within the map
    BlockMap::iterator block = m_Blocks.insert(std::make_pair(p_Address, p_Size)).first;

    // Retrieve iterators on the neighbouring blocks
    BlockMap::iterator left  = block != m_Blocks.begin() ? boost::prior(block) : block;
    BlockMap::iterator right = boost::next(block);

    // Check if we can coalesce with the block at the right
    if (right != m_Blocks.end() && block->first + block->second == right->first) {
        // Coalesce with he right block
        block->second += right->second;
        m_Blocks.erase(right);
    }

    // Check if we can coalesce with the block at the left
    if (left != block && left->first + left->second == block->first) {
        // Coalesce witht he left block
        left->second += block->second;
        m_Blocks.erase(block);
    }
}

} // namespace Utilities
} // namespace Nutshell
