//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef THREADING_MUTEX_H
#define THREADING_MUTEX_H

#include "Threading/Thread.h"
#include "Threading/SpinLock.h"
#include "Threading/Guards.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// This class encapsulates an mutex.
//******************************************************************************
class Mutex : boost::noncopyable {
private:

    // This  controls the priority boost that a thread that was waiting on the
    // mutex  gets  when he's woken  up. It should be  just  enough to prevent
    // starvation from happening among threads of the same priority.
    static const int AWAKEN_THREAD_BOOST = 1;

    Thread*             m_pOwner;       // The thread that currently owns the mutex.
    int                 m_Count;        // The number of times that the mutex has been locked.
    mutable SpinLock    m_SpinLock;     // The spin lock that protects the mutex.

public:

    // Construction / destruction
    Mutex();
    ~Mutex();

    // Mutex management
    void Lock();
    void Unlock();
};

typedef Locker<Mutex> MutexLocker;
typedef TryLocker<Mutex> MutexTryLocker;
typedef Unlocker<Mutex> MutexUnlocker;

} // namespace Threading
} // namespace Nutshell

#endif // !THREADING_MUTEX_H
