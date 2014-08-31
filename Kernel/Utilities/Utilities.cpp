//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Machine.h"
#include "Utilities/Utilities.h"
#include "Paging/Pager.h"

#include <algorithm>

int dummy()
{
/*    asm(".global _pthread_setspecific ; _pthread_setspecific:");
    asm(".global _pthread_getspecific ; _pthread_getspecific:");
    asm(".global _pthread_key_create ; _pthread_key_create:");
    asm(".global _pthread_once ; _pthread_once:");
    asm(".global _pthread_atfork ; _pthread_atfork:");
    asm(".global _pthread_create ; _pthread_create:");
    asm(".global _pthread_mutex_init ; _pthread_mutex_init:");
    asm(".global _pthread_mutex_lock ; _pthread_mutex_lock:");
    asm(".global _pthread_mutex_unlock ; _pthread_mutex_unlock:");
    asm(".global _pthread_mutex_destroy ; _pthread_mutex_destroy:");

    asm(".global _AddAtomA ; _AddAtomA:");
    asm(".global _FindAtomA ; _FindAtomA:");
    asm(".global _GetAtomNameA ; _GetAtomNameA:");

    asm(".global _sysconf ; _sysconf:");
*/

    assert(false);
}

void* operator new(size_t p_Size)
{
    return malloc(p_Size);
}

void* operator new[](size_t p_Size)
{
    return malloc(p_Size);
}

void operator delete(void* p_pPtr)
{
    free(p_pPtr);
}

void operator delete[](void* p_pPtr)
{
    free(p_pPtr);
}

//******************************************************************************
// The abort() standard call.
//******************************************************************************
extern "C" void abort()
{
    // Panic in order to halt the kernel
    PANIC("abort() has been called!");

    // To avoid a compiler warning...
    while (1);
}

//******************************************************************************
// The memcpy() standard call.
//
// Parameters:
//  p_pDest     - The destination memory.
//  p_pSource   - The source memory.
//  p_Count     - The number of bytes to copy.
//
// Returns:
//  The value of /p_pDest/.
//******************************************************************************
extern "C" void* memcpy(void* p_pDest, const void* p_pSource, size_t p_Count)
{
    assert(p_pSource != 0);
    assert(p_pDest != 0);
    assert(p_Count > 0);

    // Copy the memory from one region to the other
    for (int i = 0; i < p_Count; ++i) {
        reinterpret_cast<char*>(p_pDest)[i] = reinterpret_cast<const char*>(p_pSource)[i];
    }

    return p_pDest;
}

//******************************************************************************
// The memmove() standard call.
//
// Parameters:
//  p_pDest     - The destination memory.
//  p_pSource   - The source memory.
//  p_Count     - The number of bytes to copy.
//
// Returns:
//  The value of /p_pDest/.
//******************************************************************************
extern "C" void* memmove(void* p_pDest, const void* p_pSource, size_t p_Count)
{
    assert(p_pSource != 0);
    assert(p_pDest != 0);
    assert(p_Count > 0);

    // We act differently depending on whether we're moving up or down
    if (p_pSource > p_pDest) {
        // Copy the memory from one region to the other, in ascending order
        for (int i = 0; i < p_Count; ++i) {
            reinterpret_cast<char*>(p_pDest)[i] = reinterpret_cast<const char*>(p_pSource)[i];
        }
    } else {
        // Copy the memory from one region to the other, in descending order
        for (int i = p_Count - 1; i >= 0; --i) {
            reinterpret_cast<char*>(p_pDest)[i] = reinterpret_cast<const char*>(p_pSource)[i];
        }
    }

    return p_pDest;
}

//******************************************************************************
// The memset() standard call.
//
// Parameters:
//  p_pDest - The memory to fill.
//  p_Value - The value to fill the memory with.
//  p_Count - The number of bytes to fill.
//
// Returns:
//  The value of /p_pDest/.
//******************************************************************************
extern "C" void* memset(void* p_pDest, int p_Value, size_t p_Count)
{
    assert(p_pDest != 0);
    assert(p_Count > 0);

    // Fill the memory with the character
    for (int i = 0; i < p_Count; ++i) {
        reinterpret_cast<char*>(p_pDest)[i] = p_Value;
    }

    return p_pDest;
}

//******************************************************************************
// The memchr() standard call.
//
// Parameters:
//  p_pBuffer - The buffer to search.
//  p_Value   - The character to look for.
//  p_Count   - The number of characters to check.
//
// Returns:
//  A pointer to the first location of p_Char, or NULL if not found.
//******************************************************************************
extern "C" void* memchr(const void* p_pBuffer, int p_Value, size_t p_Count)
{
    assert(p_pBuffer != 0);

    const void* found = 0;
    const char* current = reinterpret_cast<const char*>(p_pBuffer);
    const char* end = current + p_Count;
    while (current != end) {
        if (*current == p_Value) {
            found = current;
            break;
        } else {
            ++current;
        }
    }

    return const_cast<void*>(found);
}

//******************************************************************************
// The strcpy() standard call.
//
// Parameters:
//  p_pDest   - Buffer in which to copy the string.
//  p_pSource - The string to copy.
//
// Returns:
//  The destination string.
//******************************************************************************
extern "C" char* strcpy(char* p_pDest, const char* p_pSource)
{
    // Copy the string
    char* pDest = p_pDest;
    const char* pSrc = p_pSource;
    do {
        *(pDest++) = *pSrc;
    }
    while (*(pSrc++) != 0);

    return p_pDest;
}

//******************************************************************************
// The strcmp() standard call.
//
// Parameters:
//  p_pFirst  - First string to compare.
//  p_pSecond - Second string to compare.
//
// Returns:
//  - A negative value if p_pFirst is lesser than p_pSecond.
//  - A positive value if p_pSecond is lesser than p_pFirst.
//  - 0 if both strings are equal.
//******************************************************************************
extern "C" int strcmp(const char* p_pFirst, const char* p_pSecond)
{
    assert(p_pFirst != 0);
    assert(p_pSecond != 0);

    // Compare each characters
    const char* cur1 = p_pFirst;
    const char* cur2 = p_pSecond;
    for (;;) {
        // Check for differences
        if (*cur1 < *cur2) {
            return -1;
        }
        if (*cur2 > *cur1) {
            return 1;
        }

        // If we get here, both string are equal so far.
        if (*cur1 == 0 && *cur2 == 0) {
            return 0;
        }
        if (*cur1 == 0) {
            return -1;
        }
        if (*cur2 == 0) {
            return 1;
        }

        // Advance pointers
        ++cur1;
        ++cur2;
    }
}

//******************************************************************************
// The strlen() standard call.
//
// Parameters:
//  p_pString - The string to compute the length.
//
// Returns:
//  The length of the string.
//******************************************************************************
extern "C" size_t strlen(const char* p_pString)
{
    assert(p_pString != 0);

    // Loop until we find the end of the string
    unsigned length = 0;
    while (*(p_pString++) != 0) ++length;

    return length;
}

//******************************************************************************
// The sbrk() standard call.
//
// Parameters:
//  p_Increment - The number of bytes to add to the kernel virtual space.
//
// Returns:
//  The previous end of the kernel address space.
//
// Notes:  This call  is  used  by the  malloc  implementation to  obtain more
// memory. It uses the pager to  change the size  of the kernel memory when it
// is available, and otherwise just increases the boundary.
//******************************************************************************
extern "C" void* sbrk(ssize_t p_Increment)
{
    assert(p_Increment % OS_PAGE_SIZE == 0);

    // This holds a  copy of the  kernel memory size  that is used to allocate
    // memory while  the pager isn't up yet, when initializing. The value used
    // here is the amount of memory used by the kernel.
    static size_t size = &Nutshell::_END_OF_BSS - &Nutshell::_BEGINNING_OF_TEXT;
                                                   
    // Change the size of the kernel virtual space
    size += p_Increment;

    // If the pager is up, notify him of the change
    if (Nutshell::g_pPager != 0) Nutshell::g_pPager->KernelSize(size);

    return reinterpret_cast<void*>(KERNEL_SPACE_BOUNDARY + size - p_Increment);
}

namespace Nutshell {
namespace Utilities {

//******************************************************************************
// Rounds a value to the next boundary.
//
// Parameters:
//  p_Value - The value to round.
//  p_Round - The boundary to round to.
//
// Returns:
//  The rounded value.
//******************************************************************************
size_t RoundUp(size_t p_Value, size_t p_Round)
{
    assert(p_Round != 0);

    // Return the rounded value
    size_t modulo = p_Value % p_Round;
    return modulo != 0 ? p_Value + (p_Round - modulo) : p_Value;
}

//******************************************************************************
// Checks if a specific bit is raised in an unsigned value.
//
// Parameters:
//  p_Value - The value to test.
//  p_Bit   - The number of the bit to check.
//
// Returns:
//  Whether the bit is raised.
//******************************************************************************
bool BitTest(const unsigned& p_Value, int p_Bit)
{
    assert(p_Bit >= 0);
    assert(p_Bit < 32);

    return p_Value & 1 << p_Bit;
}

//******************************************************************************
// Sets the value of a specific bit in an unsigned value.
//
// Parameters:
//  p_Dest      - The value in which to set or unset the bit.
//  p_Bit       - The number of the bit to modify.
//  p_Raised    - Whether the bit should be raised.
//******************************************************************************
void BitModify(unsigned& p_rDest, int p_Bit, bool p_Raised)
{
    assert(p_Bit >= 0);
    assert(p_Bit < 32);

    // Raise or unraise the bit
    p_rDest = p_Raised ? p_rDest | 1 << p_Bit : p_rDest & ~(1 << p_Bit);
}

//******************************************************************************
// Scans an  integer  to the right and returns  the position of the  first bit
// that is raised.
//
// Parameters:
//  p_Value - The value to scan.
//
// Returns:
//  The position of the first bit that is raised.
//******************************************************************************
int BitScanRight(unsigned p_Value)
{
#ifdef _INTEL386_
    // If the value is 0, we return -1
    if (p_Value == 0) return -1;

    // Use the BSR instruction to find the most significant bit of the value
    int position;
    asm("bsr %1, %0" : "=r" (position) : "g" (p_Value));

    return position;
#else
    #error "Not implemented!"
#endif // _INTEL386_
}

//******************************************************************************
// Scans an integer to the left  and returns the position of first bit that is
// raised.
//
// Parameters:
//  p_Value - The value to scan.
//
// Returns:
//  The position of the first bit that is raised.
//******************************************************************************
int BitScanLeft(unsigned p_Value)
{
#ifdef _INTEL386_
    // If the value is 0, we return -1
    if (p_Value == 0) return -1;

    // Use the BSF instruction to find the most significant bit of the value
    int position;
    asm("bsf %1, %0" : "=r" (position) : "g" (p_Value));

    return position;
#else
    #error "Not implemented!"
#endif // _INTEL386_
}

} // namespace Utilities
} // namespace Nutshell
