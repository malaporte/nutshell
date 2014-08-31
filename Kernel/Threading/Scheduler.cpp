//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Threading/Scheduler.h"
#include "Threading/InterruptLock.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// Constructor.
//******************************************************************************
Scheduler::Scheduler()
:   m_pCurrent(0)
{
    assert(this != 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
Scheduler::~Scheduler()
{
    assert(this != 0);
    assert(false);
}

//******************************************************************************
// Adds a thread to the scheduler.
//
// Parameters:
//  p_spThread - The thread to add to the scheduler.
//******************************************************************************
void Scheduler::AddThread(ThreadSP p_spThread)
{
    assert(this != 0);
    assert(p_spThread != 0);
    InterruptLock intlock;
    Locker<SpinLock> lock(m_SpinLock);

    // Check if this is the first thread added to the scheduler
    if (m_pCurrent == 0) {
        // Initialize the thread effective priority
        p_spThread->m_Effective = 0;
    } else {
        // Initialize the thread effective priority related to ours
        p_spThread->m_Effective  = m_pCurrent->m_Effective;
        p_spThread->m_Effective += p_spThread->m_Base - p_spThread->m_Boost;
        p_spThread->m_Effective -= m_pCurrent->m_Base - m_pCurrent->m_Boost;
    }

    // Add it to the thread vector
    m_Threads.push_back(p_spThread);

    // Add it to the ready heap
    m_Ready.push_back(p_spThread.get());
    std::push_heap(m_Ready.begin(), m_Ready.end(), Utilities::DereferenceCompare<Thread>());
}

//******************************************************************************
// Removes a thread from the scheduler.
//
// Parameters:
//  p_pThread - The thread to remove from the scheduler.
//******************************************************************************
void Scheduler::RemoveThread(Thread* p_pThread)
{
    assert(this != 0);
    assert(p_pThread != 0);
    InterruptLock intlock;
    Locker<SpinLock> lock(m_SpinLock);

    assert(false);
}

//******************************************************************************
// Handler for the clock interrupt.
//******************************************************************************
void Scheduler::Clock()
{
    // Switch to another thread
    Switch();
}

//******************************************************************************
// Switches execution to another thread.
//******************************************************************************
void Scheduler::Switch()
{
    assert(this != 0);
    InterruptLock intlock; 

    // We must hold the spin lock while we're modifying data
    {
        Locker<SpinLock> lock(m_SpinLock);

        // Check if the current thread is still ready
        if (m_pCurrent != 0 && m_pCurrent->m_State == Thread::STATE_READY) {
            // Increment the thread's effective priority
            m_pCurrent->m_Effective += m_pCurrent->m_Base;

            // Decrement it's priority boost, if any.
            if (m_pCurrent->m_Boost > 0) {
                --m_pCurrent->m_Boost;
                ++m_pCurrent->m_Effective;
            }

            // Put it back into the heap
            m_Ready.push_back(m_pCurrent);
            std::push_heap(m_Ready.begin(), m_Ready.end(), Utilities::DereferenceCompare<Thread>());
        }

        // At this time we must still have a ready thread
        assert(!m_Ready.empty());

        // The  top thread  in the queue is the one with  the lowest effective
        // priority.   Since  this  priority  always  grows,  and  to  avoid a
        // wrapping when the limit of an integer is  reached, we decrement all
        // of them at a 'regular' interval.
        if (m_Ready.front()->m_Effective >= EFFECTIVE_PRIORITY_LIMIT) {
            // Reduce the priority of all ready threads
            for (ThreadVector::iterator it = m_Ready.begin(); it != m_Ready.end(); ++it) {
                // Update the priority of the current thread
                (*it)->m_Effective -= EFFECTIVE_PRIORITY_LIMIT;
            }
        }

        // Retrieve  the  thread  with  the  best  priority  (the  one  at the
        // beginning of the vector, since the vector is a heap).
        m_pCurrent = m_Ready.front();
        std::pop_heap(m_Ready.begin(), m_Ready.end(), Utilities::DereferenceCompare<Thread>());
        m_Ready.pop_back();
    }

    // Switch execution to the new current thread
    m_pCurrent->Switch();
}

//******************************************************************************
// Makes the current thread sleep on a specific channel.
//
// Parameters:
//  p_pChannel  - The channel on which to sleep.
//  p_Boost     - The priority boost to give to the thread when it awakes.
//  p_pSpinLock - A spin lock to release and reacquire after sleeping.
//******************************************************************************
void Scheduler::Sleep(void* p_pChannel, int p_Boost, SpinLock* p_pSpinLock)
{
    assert(this != 0);
    assert(m_pCurrent != 0);
    assert(p_pChannel != 0);
    InterruptLock intlock; 

    // We must hold the spin lock while we're modifying data
    {
        Locker<SpinLock> lock(m_SpinLock);

        // Mark the current thread as sleeping
        m_pCurrent->m_State     = Thread::STATE_SLEEPING;
        m_pCurrent->m_Boost     = std::max(p_Boost, m_pCurrent->m_Boost);
        m_pCurrent->m_Effective = m_pCurrent->m_Base + m_pCurrent->m_Boost;
        m_pCurrent->m_pChannel  = p_pChannel;

        // Add it to the  sleeping threads
        m_Sleeping.push_back(m_pCurrent);

        // Unlock the specified spin lock, if any.
        if (p_pSpinLock != 0) p_pSpinLock->Unlock();
    }

    // Wait for someone to wake us up
    Switch();

    // Lock back the specified spin lock, if any.
    if (p_pSpinLock != 0) p_pSpinLock->Lock();
}

//******************************************************************************
// Wakes up all threads waiting on a specific channel.
//
// Parameters:
//  p_pChannel - The channel whose threads will be waked up.
//  p_pSpinLock - A spin lock to release and reacquire after switching.
//
// Returns:
//  The number of threads that have been waked up.
//******************************************************************************
size_t Scheduler::WakeUp(void* p_pChannel, SpinLock* p_pSpinLock)
{
    assert(this != 0);
    assert(p_pChannel != 0);
    InterruptLock intlock; 

    // This will be the number of threads we woke up
    size_t count = 0;

    // This flag will  be raised if we wake up a thread with a better priority
    // than the one of the current thread.
    bool higher = false;

    // We must hold the spin lock while we're modifying data
    {
        Locker<SpinLock> lock(m_SpinLock);

        // Go through the sleeping threads  (in reverse order  so that threads
        // that  were  waiting  for  longer gets  a  slightly  better priority
        // boost).
        for (ThreadVector::reverse_iterator it = m_Sleeping.rbegin(); it != m_Sleeping.rend(); ++it) {
            // Check if the current thread is waiting on the correct channel
            if ((*it)->m_pChannel == p_pChannel) {
                // Mark the thread as ready
                (*it)->m_State      = Thread::STATE_READY;
                (*it)->m_Boost     += count++;
                (*it)->m_Effective  = m_pCurrent->m_Effective;
                (*it)->m_Effective += (*it)->m_Base - (*it)->m_Boost;
                (*it)->m_Effective -= m_pCurrent->m_Base - m_pCurrent->m_Boost;

                // Add it to the ready heap
                m_Ready.push_back(*it);
                std::push_heap(m_Ready.begin(), m_Ready.end(), Utilities::DereferenceCompare<Thread>());

                // Check if the thread has a better priority than ours
                higher |= (*it)->m_Effective < m_pCurrent->m_Effective;

                // Mark the thread for deletion in the sleeping vector
                *it = 0;
            }
        }

        // Now remove the threads that were waked up from the vector
        std::remove(m_Sleeping.begin(), m_Sleeping.end(), static_cast<Thread*>(0));
        m_Sleeping.resize(m_Sleeping.size() - count);

        // If we're gonna switch, unlock the specified spin lock, if any.
        if (higher && p_pSpinLock != 0) p_pSpinLock->Unlock();
    }

    // If we woke up at least one thread with a better priority, let  him have
    // the control of the processor.
    if (higher) {
        // Give the processor to more prioritive threads
        Switch();

        // Lock back the specified spin lock, if any.
        if (p_pSpinLock != 0) p_pSpinLock->Lock();
    }

    return count;
}

//******************************************************************************
// Returns the current thread.
//******************************************************************************
Thread* Scheduler::Current() const
{
    assert(this != 0);

    return m_pCurrent;
}

} // namespace Threading
} // namespace Nutshell
