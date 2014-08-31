//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef GLOBAL_H
#define GLOBAL_H

#include <cassert>

namespace Nutshell {

// Panic and verification macros
#define PANIC(msg) Nutshell::Core::Panic(msg, __FILE__, __LINE__);
#define VERIFY(x) if (!(x)) PANIC("Verification failed: "#x);

// Macro useful to insert short debug-only code
#ifdef _DEBUG
#   define DEBUGCODE(x) x
#else
#   define DEBUGCODE(x)
#endif // _DEBUG

// Some global constants about the kernel
#define KERNEL_LOAD_ADDRESS     0x00100000
#define KERNEL_SPACE_BOUNDARY   0xC0000000

// Markers for the position of the kernel sections
extern "C" char     _BEGINNING_OF_TEXT;
extern "C" char     _END_OF_TEXT;
extern "C" char     _BEGINNING_OF_DATA;
extern "C" char     _END_OF_DATA;
extern "C" char     _BEGINNING_OF_BSS;
extern "C" char     _END_OF_BSS;

// Fundamental objects and their forward definitions
namespace Core { class Console; }
extern Core::Console* g_pConsole;
namespace Paging { class Pager; }
extern Paging::Pager* g_pPager;
namespace Threading { class Scheduler; }
extern Threading::Scheduler* g_pScheduler;

} // namespace Nutshell

// Some boost  classes need  the  iostream classes to be defined, event if they
// aren't implemented, so we declare them here before including headers.
namespace std {
    template <class E, class T> class basic_istream;
    template <class E, class T> class basic_ostream;
}

// Include the basic STL headers
#include <new>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/utility.hpp>

// Prototype of the memory allocation functions
extern "C" void*    malloc(size_t p_Size);
extern "C" void*    calloc(size_t p_Count, size_t p_Size);
extern "C" void*    realloc(void* p_pBlock, size_t p_Size);
extern "C" void*    memalign(size_t p_Alignment, size_t p_Size);
extern "C" void     free(void* p_pBlock);

// Include the basic headers
#include "Core/Panic.h"
#include "Core/Console.h"
#include "Utilities/Utilities.h"

#endif // !GLOBAL_H
