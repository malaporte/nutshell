//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Machine.h"
#include "Threading/InterruptLock.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// Constructor.
//******************************************************************************
InterruptLock::InterruptLock()
{
    assert(this != 0);

    // Disable interrupts
    m_Enabled = Machine::DisableInterrupts();
}

//******************************************************************************
// Destructor.
//******************************************************************************
InterruptLock::~InterruptLock()
{
    assert(this != 0);

    // Restore the previous interrupt state
    if (m_Enabled) Machine::EnableInterrupts();
}

} // namespace Threading
} // namespace Nutshell
