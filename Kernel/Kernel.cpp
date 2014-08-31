//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

// This _must_ be  at  the  top of the file, since it is  the first instruction
// that is executed. It transfer control to the machine dependent init code.
asm(".text ; jmp _MachineStartup");

#include "Global.h"
#include "Machine.h"
#include "Paging/Pager.h"
#include "Threading/Scheduler.h"
#include "Threading/Process.h"
#include "Threading/Thread.h"
#include "Intel386/Intel386.h"

namespace Nutshell {

// From "Global.h"
Core::Console*          g_pConsole = 0;
Paging::Pager*          g_pPager = 0;
Threading::Scheduler*   g_pScheduler = 0;

// The global constructor and destructor tables
extern "C" void (*_CTOR_LIST[])();
extern "C" void (*_DTOR_LIST[])();

// The system process entry point
extern "C" void Main(void*);

//******************************************************************************
// Global kernel entry point.
//
// This is  called after the machine interface is initialized.  At  this point,
// we  should have a  completely  valid working  environment. We must now start
// the basic kernel systems and then launch the system process.
//******************************************************************************
extern "C" void start()
{
    // Call all global constructors
    for (int i = 0; _CTOR_LIST[i] != 0; ++i) {
        _CTOR_LIST[i]();
    }

    g_pConsole = new Core::Console();

    PANIC("Stop here");

    // Create the pager
    g_pPager = new Paging::Pager();

    // Create the scheduler.
    g_pScheduler = new Threading::Scheduler();

    // All basic components are now created. We may start the system process.

    // Create the system process.
    Threading::ProcessSP spProcess(new Threading::Process());
    g_pPager->AddPageable(spProcess);

    // Create the first thread in the process.
    Threading::ThreadSP  spThread(new Threading::Thread(spProcess, (void (*)(void*)) &Main));
    g_pScheduler->AddThread(spThread);

    PANIC("About to switch!");

    // Switch to the system process. Should never come back.
    g_pScheduler->Switch();
    PANIC("Came back from first task switch!");
}

} // namespace Nutshell
