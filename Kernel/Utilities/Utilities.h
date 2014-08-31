//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef UTILITIES_UTILITIES_H
#define UTILITIES_UTILITIES_H

// Standard C/C++ and library support calls
extern "C" void     abort();
extern "C" void*    memcpy(void* p_pDest, const void* p_pSource, size_t p_Count);
extern "C" void*    memmove(void* p_pDest, const void* p_pSource, size_t p_Count);
extern "C" void*    memset(void* p_pDest, int p_Value, size_t p_Count);
extern "C" char*    strcpy(char* p_pDest, const char* p_pSource);
extern "C" int      strcmp(const char* p_pFirst, const char* p_pSecond);
extern "C" size_t   strlen(const char* p_pString);
extern "C" void*    sbrk(ssize_t p_Increment);

namespace Nutshell {
namespace Utilities {

// Rounding utilities
size_t  RoundUp(size_t p_Value, size_t p_Round);

// Bit manipulation utilities
bool BitTest(const unsigned& p_Value, int p_Bit);
void BitModify(unsigned& p_rDest, int p_Bit, bool p_Raised);
int BitScanRight(unsigned p_Value);
int BitScanLeft(unsigned p_Value);

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Thread safe variable manipulation functions.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Exchanges the value of a variable with another one.
//
// Parameters:
//  p_rValue    - The variable to exchange with.
//  p_NewValue  - The value to put in the variable instead.
//
// Returns:
//  The old value of the variable.
//******************************************************************************
template <typename TYPE>
inline TYPE ThreadSafeExchange(TYPE& p_rValue, TYPE p_NewValue)
{
    assert(reinterpret_cast<unsigned>(&p_rValue) % sizeof(int) == 0);

#ifdef _INTEL386_
    // Use an assembler instruction to perform the operation
    asm("lock xchgl %1, %0" : "=m" (p_rValue), "=r" (p_NewValue) : "1" (p_NewValue));

    return p_NewValue;
#else
    #error "Not implemented!"
#endif // _INTEL_386_
}

//******************************************************************************
// Exchanges  the value  of a variable  with another one if it's current value
// is equal to another one.
//
// Parameters:
//  p_rValue    - The variable to exchange with.
//  p_NewValue  - The value to put in the variable instead.
//  p_Compare   - The value to compare the current one with.
//
// Returns:
//  The old value of the variable.
//******************************************************************************
template<typename TYPE, typename COMPARE>
inline COMPARE ThreadSafeCompareExchange(TYPE& p_rValue, TYPE p_NewValue, COMPARE p_Compare)
{
    assert(reinterpret_cast<unsigned>(&p_rValue) % sizeof(int) == 0);
    assert(sizeof(TYPE) == sizeof(int));

#ifdef _INTEL386_
    // Use an assembler instruction to perform the operation
    asm("lock cmpxchgl %2, %0" : "=m" (p_rValue), "=a" (p_Compare) : "r" (p_NewValue), "1" (p_Compare));

    return p_Compare;
#else
    #error "Not implemented!"
#endif // _INTEL_386_
}

//******************************************************************************
// Adds a value to a variable.
//
// Parameters:
//  p_rValue - The variable to add to.
//  p_Add    - The value to add.
//
// Returns:
//  The old value of the variable.
//******************************************************************************
template <typename TYPE>
inline TYPE ThreadSafeAdd(TYPE& p_rValue, TYPE p_Add)
{
    // TODO: Maybe implement directly in assembly code.
    for (;;) {
        TYPE old = p_rValue;
        TYPE expected = old + p_Add;
        if (ThreadSafeCompareExchange(p_rValue, expected, old) == expected) {
            return expected;
        }
    }
}

//******************************************************************************
// Substracts a value to a variable.
//
// Parameters:
//  p_rValue - The variable to substract to.
//  p_Sub    - The value to substract.
//
// Returns:
//  The old value of the variable.
//******************************************************************************
template <typename TYPE>
inline TYPE ThreadSafeSubstract(TYPE& p_rValue, TYPE p_Sub)
{
    ThreadSafeAdd(p_rValue, -p_Sub);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Some useful misceallenous utilities.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//******************************************************************************
// Class that allows the comparison of dereferenced pointers.
//******************************************************************************
template<typename Type, typename Compare = std::less<Type> >
struct DereferenceCompare
{
    bool operator()(const Type* p_pFirst, const Type* p_pSecond) { return Compare()(*p_pFirst, *p_pSecond); };
};

//******************************************************************************
// Predicate that checks if a value is within a sequence.
//******************************************************************************
template<typename Sequence, typename Type> 
inline bool SequenceContains(const Sequence& p_Sequence, const Type& p_Value)
{
    return std::find(p_Sequence.begin(), p_Sequence.end(), p_Value) != p_Sequence.end();
}

} // namespace Utilities
} // namespace Nutshell

#endif // !UTILITIES_UTILITIES_H
