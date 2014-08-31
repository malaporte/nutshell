//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef THREADING_INTERRUPTLOCK_H
#define THREADING_INTERRUPTLOCK_H

namespace Nutshell {
namespace Threading {

//******************************************************************************
// This class disables interrupts until it is destroyed.
//******************************************************************************
class InterruptLock : boost::noncopyable {
private:

    bool    m_Enabled;  // Whether interrupts were previously enabled.

public:

    // Construction / destruction
    InterruptLock();
    ~InterruptLock();
};

} // namespace Threading
} // namespace Nutshell

#endif // !THREADING_INTERRUPTLOCK_H
