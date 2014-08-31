//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Threading/Mutex.h"
#include "Threading/Scheduler.h"
#include "Threading/InterruptLock.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// Constructor.
//******************************************************************************
Mutex::Mutex()
:   m_pOwner(0),
    m_Count(0)
{
    assert(this != 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
Mutex::~Mutex()
{
    assert(this != 0);
}

//******************************************************************************
// Locks the mutex.
//******************************************************************************
void Mutex::Lock()
{
    assert(this != 0);
    InterruptLock intlock;
    Locker<SpinLock> lock(m_SpinLock);

    // Check if we currently own the mutex
    if (m_pOwner != g_pScheduler->Current()) {
        // Loop until we were able to acquire the mutex
        while (Utilities::ThreadSafeCompareExchange(m_pOwner, g_pScheduler->Current(), 0) != 0) {
            // Wait for something to happen to the mutex
            g_pScheduler->Sleep(this, AWAKEN_THREAD_BOOST, &m_SpinLock);
        }
    }

    // When we get here, the mutex should be ours.
    assert(m_pOwner == g_pScheduler->Current());

    // Increment the lock count
    ++m_Count;
}

//******************************************************************************
// Unlocks the mutex.
//******************************************************************************
void Mutex::Unlock()
{
    assert(this != 0);
    assert(m_pOwner == g_pScheduler->Current());
    InterruptLock intlock;
    Locker<SpinLock> lock(m_SpinLock);

    // Decrement the lock count
    --m_Count;

    // If the lock count has reached 0, release the mutex.
    if (m_Count == 0) {
        // Release the mutex
        m_pOwner = 0;
        g_pScheduler->WakeUp(this, &m_SpinLock);
    }
}

} // namespace Threading
} // namespace Nutshell
