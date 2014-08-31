//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef THREADING_THREAD_H
#define THREADING_THREAD_H

#include "Threading/Process.h"
#include "Threading/SpinLock.h"
#include "Paging/Pager.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// This class encapsulates a thread.
//******************************************************************************
class Thread {
private:

    // The size of thread's kernel stack (in bytes)
    static const size_t     KERNEL_STACK_SIZE   = 16 * 4096;

    // The various states a thread can have.
    enum States {
        STATE_READY         = 0,
        STATE_SLEEPING      = 1
    };

    // The various priorities a thread can have.
    enum Priorities {
        PRIORITY_VERY_LOW   = 100,
        PRIORITY_LOW        = 75,
        PRIORITY_NORMAL     = 50,
        PRIORITY_HIGH       = 25,
        PRIORITY_REALTIME   = 0
    };

    typedef std::vector<void*> SpecificValueVector;
    typedef std::vector<void (*)(void*)> SpecificDestructorVector;
    typedef std::vector<int> SpecificIndexVector;

    ProcessWP                       m_wpProcess;            // The process that owns the thread.
    size_t                          m_Task;                 // The task descriptor of the thread.
    Paging::MapableSP               m_spKernelStack;        // The mapable for the kernel stack.

    States                          m_State;                // The state of the thread.
    Priorities                      m_Base;                 // The base priority of the thread.
    int                             m_Boost;                // The priority boost of the thread.
    int                             m_Effective;            // The effective priority of the thread.
    void*                           m_pChannel;             // The channel on which the thread is sleeping.

    SpecificValueVector             m_Specifics;            // Vector of thread specific values.
    static SpecificDestructorVector s_SpecificDestructors;  // Vector of destructors for specific indexes.
    static SpecificIndexVector      s_AvailableSpecifics;   // Vector of available specific indexes.
    static SpinLock                 s_SpecificsLock;        // Lock that protects the thread specific members.

    friend class Scheduler;

public:

    // Construction / destruction
    Thread(ProcessSP p_spProcess, void (* p_pEntry)(void*), void* p_pArgument = 0);
    ~Thread();

    // Thread management
    void Switch();

    // Thread information
    Process*    OwnerProcess() const;
    size_t      Task();

    // Thread specific values
    static int  AllocateSpecific(void (* p_pDestructor)(void*) = 0);
    static void ReleaseSpecific(int p_Index);
    void*       Specific(int p_Index) const;
    void        Specific(int p_Index, void* p_pValue);

    // Operators
    bool operator<(const Thread& p_Thread) const;
};

typedef boost::shared_ptr<Thread> ThreadSP;

} // namespace Threading
} // namespace Nutshell

#endif // !THREADING_THREAD_H
