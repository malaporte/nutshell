//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_HANDLERS_H
#define INTEL386_HANDLERS_H

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// Interrupt handler functions.
//******************************************************************************

void DivideErrorInterruptHandler(void* p_pEIP, void*);
void DebugInterruptHandler(void* p_pEIP, void*);
void BreakpointInterruptHandler(void* p_pEIP, void*);
void OverflowInterruptHandler(void* p_pEIP, void*);
void BoundRangeExceededInterruptHandler(void* p_pEIP, void*);
void InvalidOpcodeInterruptHandler(void* p_pEIP, void*);
void DeviceNotAvailableInterruptHandler(void* p_pEIP, void*);
void DoubleFaultInterruptHandler(void* p_pEIP, void* p_pParam);
void CoprocessorSegmentOverrunInterruptHandler(void* p_pEIP, void*);
void InvalidTSSInterruptHandler(void* p_pEIP, void* p_pParam);
void SegmentNotPresentInterruptHandler(void* p_pEIP, void* p_pParam);
void StackSegmentFaultInterruptHandler(void* p_pEIP, void* p_pParam);
void GeneralProtectionInterruptHandler(void* p_pEIP, void* p_pParam);
void PageFaultInterruptHandler(void* p_pEIP, void* p_pParam);
void FloatingPointErrorInterruptHandler(void* p_pEIP, void*);
void AlignmentCheckInterruptHandler(void* p_pEIP, void* p_pParam);
void MachineCheckInterruptHandler(void* p_pEIP, void*);
void StreamingSIMDInterruptHandler(void* p_pEIP, void*);
void ClockInterruptHandler(void* p_pEIP, void*);

} // namespace Intel386
} // namespace Nutshell

#endif // !INTEL386_HANDLERS_H
