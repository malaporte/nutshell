//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Machine.h"
#include "Threading/Thread.h"
#include "Paging/Pager.h"

namespace Nutshell {
namespace Threading {

Thread::SpecificDestructorVector Thread::s_SpecificDestructors;
Thread::SpecificIndexVector Thread::s_AvailableSpecifics;
SpinLock Thread::s_SpecificsLock;

//******************************************************************************
// Constructor.
//
// Parameters:
//  p_spProcess - The process that owns the thread.
//  p_pEntry    - The address of the entry point of the thread.
//  p_pArgument - The argument passed to the thread.
//******************************************************************************
Thread::Thread(ProcessSP p_spProcess, void (* p_pEntry)(void*), void* p_pArgument)
:   m_wpProcess(p_spProcess),
    m_Task(),
    m_spKernelStack(new Paging::Mapable(KERNEL_STACK_SIZE)),
    m_State(STATE_READY),
    m_Base(PRIORITY_NORMAL),
    m_Boost(0),
    m_Effective(),
    m_pChannel(0),
    m_Specifics()
{
    assert(this != 0);
    assert(p_spProcess != 0);
    assert(p_pEntry != 0);

    // Lock the kernel stack into memory and map it within the process
    g_pPager->LockMapable(m_spKernelStack);
    size_t stack = p_spProcess->Map(m_spKernelStack);

    // Allocate a task descriptor for the thread
    // TODO: Check if it is valid!
    // TODO: Make p_pArgument work !
    m_Task = Machine::AllocateTaskDescriptor(p_spProcess->Directory(), stack + KERNEL_STACK_SIZE, p_pEntry);
}

//******************************************************************************
// Destructor.
//******************************************************************************
Thread::~Thread()
{
    assert(this != 0);

    // Call all thread specific destructors
    {
        SpinLockLocker lock(s_SpecificsLock);
        assert(m_Specifics.size() >= s_SpecificDestructors.size());
        for (int i = 0; i < s_SpecificDestructors.size(); ++i) {
            // If we have a destructor for this index, call it
            if (s_SpecificDestructors[i] != 0) {
                s_SpecificDestructors[i](m_Specifics[i]);
            }
        }
    }

    // Unmap the kernel stack from the process
    boost::make_shared(m_wpProcess)->Unmap(m_spKernelStack);

    // Release our task descriptor
    Machine::ReleaseTaskDescriptor(m_Task);
}

//******************************************************************************
// Switches execution to this thread.
//******************************************************************************
void Thread::Switch()
{
    // Switch execution to our descriptor
    if (Machine::GetCurrentTaskDescriptor() != m_Task) {
        Machine::SwitchToTaskDescriptor(m_Task);
    }
}

//******************************************************************************
// Returns the process that owns the thread.
//******************************************************************************
Process* Thread::OwnerProcess() const
{
    assert(this != 0);

    return m_wpProcess.lock().get();
}

//******************************************************************************
// Returns the task descriptor of the thread.
//******************************************************************************
size_t Thread::Task()
{
    assert(this != 0);

    return m_Task;
}

//******************************************************************************
// Allocates a specific index.
//
// Parameters:
//  p_pDestructor - Destructor to call upon each thread exit.
//
// Returns:
//  The newly allocated index.
//******************************************************************************
int Thread::AllocateSpecific(void (* p_pDestructor)(void*))
{
    SpinLockLocker lock(s_SpecificsLock);

    // Look for a released one
    int index;
    if (!s_AvailableSpecifics.empty()) {
        index = s_AvailableSpecifics.back();
        s_AvailableSpecifics.pop_back();
        assert(index < s_SpecificDestructors.size());
        s_SpecificDestructors[index] = p_pDestructor;
    } else {
        // Allocate a brand new index
        index = s_SpecificDestructors.size();
        s_SpecificDestructors.push_back(p_pDestructor);
    }

    return index;
}

//******************************************************************************
// Releases an allocated thread specific index.
//
// Parameters:
//  p_Index - The index to release.
//******************************************************************************
void Thread::ReleaseSpecific(int p_Index)
{
    assert(p_Index < s_SpecificDestructors.size());
    SpinLockLocker lock(s_SpecificsLock);
    assert(!Utilities::SequenceContains(s_AvailableSpecifics, p_Index));

    // Put the index in the available list
    s_AvailableSpecifics.push_back(p_Index);
}

//******************************************************************************
// Returns a thread specific value.
//
// Parameters:
//  p_Index - The index of the thread specific value.
//
// Returns:
//  The thread specific value.
//******************************************************************************
void* Thread::Specific(int p_Index) const
{
    assert(this != 0);

    return m_Specifics[p_Index];
}

//******************************************************************************
// Sets a thread specific value.
//
// Parameters:
//  p_Index  - The index of the thread specific value.
//  p_pValue - The thread specific value.
//******************************************************************************
void Thread::Specific(int p_Index, void* p_pValue)
{
    assert(this != 0);

    m_Specifics[p_Index] = p_pValue;
}

//******************************************************************************
// Operator <.
//
// Parameters:
//  p_Thread - The thread to compare to.
//
// Returns:  Whether our effective priority is the lowest (which means
// that it is higher than that of /p_Thread/).
//******************************************************************************
bool Thread::operator<(const Thread& p_Thread) const
{
    assert(this != 0);

    return m_Effective > p_Thread.m_Effective;
}

} // namespace Threading
} // namespace Nutshell
