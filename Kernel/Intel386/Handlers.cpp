//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/Handlers.h"
#include "Intel386/Intel386.h"
#include "Intel386/Interrupts.h"
#include "Paging/Pager.h"
#include "Threading/Scheduler.h"

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// Internal handler for the divide error interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void DivideErrorInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Divide error interrupt!");
}

//******************************************************************************
// Internal handler for the debug interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void DebugInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Debug interrupt!");
}

//******************************************************************************
// Internal handler for the breakpoint interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void BreakpointInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Breakpoint interrupt!");
}

//******************************************************************************
// Internal handler for the overflow interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void OverflowInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Overflow interrupt!");
}

//******************************************************************************
// Internal handler for the bound range exceeded interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void BoundRangeExceededInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Bound range exceeded interrupt!");
}

//******************************************************************************
// Internal handler for the invalid opcode interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void InvalidOpcodeInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Invalid opcode interrupt!");
}

//******************************************************************************
// Internal handler for the device not available interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void DeviceNotAvailableInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Device not available interrupt!");
}

//******************************************************************************
// Internal handler for the double fault interrupt.
//
// Parameters:
//  p_pEIP   - The address of the faulty instruction.
//  p_pParam - The exception parameter.
//******************************************************************************
void DoubleFaultInterruptHandler(void* p_pEIP, void* p_pParam)
{
    PANIC("Double fault interrupt!");
}

//******************************************************************************
// Internal handler for the coprocessor segment overrun interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void CoprocessorSegmentOverrunInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Coprocessor segment overrun interrupt!");
}

//******************************************************************************
// Internal handler for the invalid TSS interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//  p_pParam - The exception parameter.
//******************************************************************************
void InvalidTSSInterruptHandler(void* p_pEIP, void* p_pParam)
{
    PANIC("Invalid TSS interrupt!");
}

//******************************************************************************
// Internal handler for the segment not present interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//  p_pParam - The exception parameter.
//******************************************************************************
void SegmentNotPresentInterruptHandler(void* p_pEIP, void* p_pParam)
{
    PANIC("Segment not present interrupt!");
}

//******************************************************************************
// Internal handler for the stack segment fault interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//  p_pParam - The exception parameter.
//******************************************************************************
void StackSegmentFaultInterruptHandler(void* p_pEIP, void* p_pParam)
{
    PANIC("Stack segment interrupt!");
}

//******************************************************************************
// Internal handler for the general protection interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//  p_pParam - The exception parameter.
//******************************************************************************
void GeneralProtectionInterruptHandler(void* p_pEIP, void* p_pParam)
{
    PANIC("General protection interrupt!");
}

//******************************************************************************
// Internal handler for the page fault interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//  p_pParam - The exception parameter.
//******************************************************************************
void PageFaultInterruptHandler(void* p_pEIP, void* p_pParam)
{
    // Retrieve the address that caused the page fault
    size_t address;
    asm("movl %%cr2, %0" : "=r" (address));

    // Call the page fault handler on the pager
    g_pPager->PageFault(address);
}

//******************************************************************************
// Internal handler for the floating point error interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void FloatingPointErrorInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Floating point error interrupt!");
}

//******************************************************************************
// Internal handler for the alignment check interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//  p_pParam - The exception parameter.
//******************************************************************************
void AlignmentCheckInterruptHandler(void* p_pEIP, void* p_pParam)
{
    PANIC("Alignment check interrupt!");
}

//******************************************************************************
// Internal handler for the machine check interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void MachineCheckInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Machine check interrupt!");
}

//******************************************************************************
// Internal handler for the streaming SIMD interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void StreamingSIMDInterruptHandler(void* p_pEIP, void*)
{
    PANIC("Streaming SIMD interrupt!");
}

//******************************************************************************
// Internal handler for the clock interrupt.
//
// Parameters:
//  p_pEIP - The address of the faulty instruction.
//******************************************************************************
void ClockInterruptHandler(void* p_pEIP, void*)
{
    // Send the End Of Interrupt to the PIC
    SendEndOfInterrupt(RTC_INTERRUPT);

    // Read the  C status  register  from the clock to  know why the interrupt
    // has been generated. This also does the acknowledge.
    OutPort(RTC_BASE_PORT, 0xC);
    int status = InPort(RTC_BASE_PORT + 1);

    // Check if the interrupt is the periodic one
    if (status & (1 << 6)) {
        // Call the clock handler on the scheduler
        g_pScheduler->Clock();
    }
}

} // namespace Intel386
} // namespace Nutshell
