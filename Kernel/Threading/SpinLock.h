//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef THREADING_SPINLOCK_H
#define THREADING_SPINLOCK_H

#include "Threading/Guards.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// This class encapsulates a spin lock.
//******************************************************************************
class SpinLock : boost::noncopyable {
private:

    int     m_Owner;    // The task descriptor that owns the spin lock.
    int     m_Count;    // The number of times we've been locked.

public:

    // Construction / destruction
    SpinLock();
    ~SpinLock();

    // Spinlock management
    void    Lock();
    bool    TryLock();
    void    Unlock();

    // Spinlock information
    size_t  Count() const;
};

typedef Locker<SpinLock> SpinLockLocker;
typedef TryLocker<SpinLock> SpinLockTryLocker;
typedef Unlocker<SpinLock> SpinLockUnlocker;

} // namespace Threading
} // namespace Nutshell

#endif // !THREADING_SPINLOCK_H
