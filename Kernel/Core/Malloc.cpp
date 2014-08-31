//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Threading/SpinLock.h"
#include "Paging/Pager.h"
#include "Intel386/Paging.h"

// This file includes the malloc implementation we use (Doug Lea's malloc.)

namespace Nutshell {
namespace Core {

namespace {

    // Use a spinlock to protect kernel memory allocations.
    Nutshell::Threading::SpinLock g_MallocLock;

} // anonymous namespace

//******************************************************************************
// This is called before each call to malloc/free/etc..
//******************************************************************************
int MallocPreAction()
{
    // Take the lock
    g_MallocLock.Lock();

    return 0;
}

//******************************************************************************
// This is called after each call to malloc/free/etc..
//******************************************************************************
int MallocPostAction()
{
    // Release the lock
    g_MallocLock.Unlock();

    // If the pager is up, have it prepare himself for the next allocation
    if (Nutshell::g_pPager != 0) Nutshell::g_pPager->PrepareNextKernelSize();

    return 0;
}

//******************************************************************************
// This is called when a malloc fails (because of no more memory).
//******************************************************************************
void MallocFailureAction()
{
    PANIC("Kernel malloc failure!");
}

} // namespace Core
} // namespace Nutshell

// Instruct malloc to use the functions defined above
#define MALLOC_PREACTION Nutshell::Core::MallocPreAction()
#define MALLOC_POSTACTION Nutshell::Core::MallocPostAction()
#define MALLOC_FAILURE_ACTION Nutshell::Core::MallocFailureAction()
#define USE_PUBLIC_MALLOC_WRAPPERS 1

// We do not want to use mmap (yet)
#define HAVE_MMAP 0

// Enable debug mode
#ifdef _DEBUG
#define DEBUG 1
#endif

// Include the malloc implementation.
extern "C" {
#define LACKS_TIME_H
#define malloc_getpagesize OS_PAGE_SIZE
#include "../../External/malloc.c"
}
