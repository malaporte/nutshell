//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Machine.h"
#include "Threading/SpinLock.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// Constructor.
//******************************************************************************
SpinLock::SpinLock()
:   m_Owner(0),
    m_Count(0)
{
    assert(this != 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
SpinLock::~SpinLock()
{
    assert(this != 0);
    assert(m_Owner == 0);
}

//******************************************************************************
// Locks the spin lock.
//******************************************************************************
void SpinLock::Lock()
{
    assert(this != 0);
    assert(!Machine::DisableInterrupts());

    // Check if we already got the lock
    int descriptor = Machine::GetCurrentTaskDescriptor();
    if (m_Owner != descriptor) {
        // Loop until we own the lock
        while (Utilities::ThreadSafeCompareExchange(m_Owner, Machine::GetCurrentTaskDescriptor(), 0) != 0);
    }

    // When we get here we have the lock
    assert(m_Owner == descriptor);
    ++m_Count;
}

//******************************************************************************
// Attempts to lock the spin lock.
//
// Returns:
//  Whether we got the lock.
//******************************************************************************
bool SpinLock::TryLock()
{
    assert(this != 0);
    assert(!Machine::DisableInterrupts());

    // Check if we already got the lock
    int descriptor = Machine::GetCurrentTaskDescriptor();
    if (m_Owner != descriptor) {
        // Attempt to get the lock once
        if (Utilities::ThreadSafeCompareExchange(m_Owner, Machine::GetCurrentTaskDescriptor(), 0) == 0) {
            ++m_Count;
            return true;
        }
    } else {
        // Just increment our lock count
        ++m_Count;
        return true;
    }

    // If we get here we don't have the lock
    return false;
}

//******************************************************************************
// Unlocks the spin lock.
//******************************************************************************
void SpinLock::Unlock()
{
    assert(this != 0);
    assert(!Machine::DisableInterrupts());
    assert(m_Owner == Machine::GetCurrentTaskDescriptor());

    // Release the lock if the lock count drops to zero
    if (--m_Count == 0) {
        m_Owner = 0;
    }
}

//******************************************************************************
// Returns the number of times the spinlock have been recursively locked.
//******************************************************************************
size_t SpinLock::Count() const
{
    assert(this != 0);
    assert(m_Owner == Machine::GetCurrentTaskDescriptor());
    return m_Count;
}

} // namespace Threading
} // namespace Nutshell
