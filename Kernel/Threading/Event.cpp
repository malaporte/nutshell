//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Threading/Event.h"
#include "Threading/Scheduler.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// Constructor.
//******************************************************************************
Event::Event()
{
    assert(this != 0);
}

//******************************************************************************
// Destructor.
//******************************************************************************
Event::~Event()
{
    assert(this != 0);
}

//******************************************************************************
// Signals the event.
//******************************************************************************
void Event::Signal()
{
    assert(this != 0);

    // Wake up schedulables sleeping on the event
    g_pScheduler->WakeUp(this);
}

//******************************************************************************
// Waits for the event to be signaled.
//******************************************************************************
void Event::Wait()
{
    assert(this != 0);

    // Wait until we're waked up
    g_pScheduler->Sleep(this);
}

} // namespace Threading
} // namespace Nutshell
