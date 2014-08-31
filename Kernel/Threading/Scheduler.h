//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef THREADING_SCHEDULER_H
#define THREADING_SCHEDULER_H

#include "Threading/Thread.h"
#include "Threading/SpinLock.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// This class encapsulates the scheduler.
//******************************************************************************
class Scheduler
:   boost::noncopyable
{
private:

    // When the  lowest effective priority reachs this limit, all threads gets
    // their effective priority truncated to avoid wrapping.
    static const int    EFFECTIVE_PRIORITY_LIMIT    = 1000;

    typedef std::vector<ThreadSP> ThreadSPVector;
    typedef std::vector<Thread*>  ThreadVector;

    ThreadSPVector      m_Threads;      // Vector that contains all the threads.
    ThreadVector        m_Ready;        // Heap of ready threads.
    ThreadVector        m_Sleeping;     // Vector of sleeping threads.
    Thread*             m_pCurrent;     // Pointer to the current thread.
    mutable SpinLock    m_SpinLock;     // The spin lock that protects the scheduler.

public:

    // Construction / destruction
    Scheduler();
    ~Scheduler();

    // Thread list management
    void AddThread(ThreadSP p_spThread);
    void RemoveThread(Thread* p_pThread);

    // Interrupts handlers
    void Clock();

    // Basic synchronization primitives
    void   Switch();
    void   Sleep(void* p_pChannel, int p_Boost = 0, SpinLock* p_pSpinLock = 0);
    size_t WakeUp(void* p_pChannel, SpinLock* p_pSpinLock = 0);

    // Misceallenous
    Thread* Current() const;
};

} // namespace Threading
} // namespace Nutshell

#endif // !THREADING_SCHEDULER_H
