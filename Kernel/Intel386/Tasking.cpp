//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/Tasking.h"
#include "Intel386/Intel386.h"
#include "Intel386/Paging.h"

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// Allocates a new task descriptor.
//
// Parameters:
//  p_Directory - The descriptor of the page directory.
//  p_Stack     - The address of the top of the stack.
//  p_pEntry    - The entry point of the task.
//
// Returns:
//  The task descriptor.
//******************************************************************************
int AllocateTaskDescriptor(size_t p_Directory, size_t p_Stack, void (* p_pEntry)(void*))
{
    assert(p_Directory != 0);
    assert(p_pEntry != 0);

    // Look for an available descriptor in the GDT
    int index = 0;
    for (int i = 0; i < 8192; ++i) {
        // Check if the current descriptor is available
        if (!(*g_pGDT)[FIRST_TSS_SEGMENT + i].P()) {
            index = FIRST_TSS_SEGMENT + i;
            break;
        }
    }

    // If we found no descriptor we're in trouble !
    if (index == 0) {
        PANIC("Out of task descriptors!");
    }

    // Allocate a new task state segment
    TaskStateSegment* pTSS = new TaskStateSegment();

    // Make the descriptor point to our the allocated TSS
    (*g_pGDT)[index].Base(reinterpret_cast<unsigned>(pTSS));
    (*g_pGDT)[index].Limit(sizeof(TaskStateSegment) - 1);
    (*g_pGDT)[index].G(true);
    (*g_pGDT)[index].P(true);
    (*g_pGDT)[index].Type(SegmentDescriptor::TSS32BitAvailable);

    // Initialize the TSS for the task
    pTSS->ESP(p_Stack);
    pTSS->SS(KERNEL_DATA_SEGMENT << 3);
    pTSS->CS(KERNEL_CODE_SEGMENT << 3);
    pTSS->DS(KERNEL_DATA_SEGMENT << 3);
    pTSS->ES(KERNEL_DATA_SEGMENT << 3);
    pTSS->FS(KERNEL_DATA_SEGMENT << 3);
    pTSS->GS(KERNEL_DATA_SEGMENT << 3);
    pTSS->EIP(reinterpret_cast<unsigned>(p_pEntry));
    pTSS->CR3(GetPhysicalAddress(reinterpret_cast<PageDirectory*>(p_Directory)));

    return index;
}

//******************************************************************************
// Releases a task descriptor.
//
// Parameters:
//  p_Task - The task descriptor.
//******************************************************************************
void ReleaseTaskDescriptor(int p_Task)
{
    assert(p_Task >= FIRST_TSS_SEGMENT && p_Task < 8192);
    assert((*g_pGDT)[p_Task].P());

    // Release the TSS for the task descriptor
    delete reinterpret_cast<TaskStateSegment*>((*g_pGDT)[p_Task].Base());

    // Reset the entry in the GDT
    (*g_pGDT)[p_Task].Reset();
}

//******************************************************************************
// Returns the current task descriptor.
//******************************************************************************
int GetCurrentTaskDescriptor()
{
    // NOTE: The /str/ instruction  used  here  does  NOT  clear the upper
    // 16-bits  when the operand is a memory location, and that is  why we
    // force the use of a register instead.

    // Retrieve the value of the task register
    unsigned short descriptor;
    asm("str %0" : "=r" (descriptor));

    return descriptor >> 3;
}

//******************************************************************************
// Switches execution to another task.
//
// Parameters:
//  p_Task - The task descriptor.
//******************************************************************************
void SwitchToTaskDescriptor(int p_Task)
{
    assert(p_Task >= FIRST_TSS_SEGMENT && p_Task < 8192);
    assert((*g_pGDT)[p_Task].P());

    // If we're currently  tracing the  kernel, we  must raise  the instruction
    // trap flag in the soon-to-be-switched-to task descriptor.
//    if (g_TracingKernel) {
//        TaskStateSegment* pTSS = reinterpret_cast<TaskStateSegment*>((*g_pGDT)[p_Task].Base());
//        pTSS->EFLAGS(pTSS->EFLAGS() | 0x100);
//    }

    // Declare a structure that will be used to call LJMP
    struct {
        unsigned        g_Offset   __attribute__((packed));
        unsigned short  g_Selector __attribute__((packed));
    } ljmpParams;

    // Fill the structure with  the appropriate values (the  g_Offset part
    // is ignored by the processor and thus we need not to fill it)
    ljmpParams.g_Selector = p_Task << 3;

    // Perform a jump to the task
    asm("ljmp *%0" : : "m" (ljmpParams));
}

} // namespace Intel386
} // namespace Nutshell
