//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef THREADING_EVENT_H
#define THREADING_EVENT_H

#include "Threading/SpinLock.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// This class encapsulates an event.
//******************************************************************************
class Event {
private:

    int     m_WaitCount;    // The number of schedulables that are waiting on the event.
    int     m_ReleaseCount; // The number of schedulables that should be released.

public:

    // Construction / destruction
    Event();
    ~Event();

    // Event manipulation
    void Signal();
    void Wait();
};

} // namespace Threading
} // namespace Nutshell

#endif // !THREADING_EVENT_H
