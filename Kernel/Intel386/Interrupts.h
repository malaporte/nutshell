//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_INTERRUPTS_H
#define INTEL386_INTERRUPTS_H

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// Interrupt management functions.
//******************************************************************************

void            InstallInterruptHandler(int p_Vector, void (* p_pFunction)(void*, void*), bool p_Parameter);
void            InstallRawInterruptHandler(int p_Vector, void* p_pFunction);
void            UninstallInterruptHandler(int p_Vector);
void            InstallHardwareInterruptHandler(int p_Interrupt, void (*p_pFunction)(void*, void*));
void            UninstallHardwareInterruptHandler(int p_Interrupt);
void            EnableInterrupts();
bool            DisableInterrupts();
unsigned short  GetInterruptMask();
void            SetInterruptMask(unsigned short p_Mask);
void            SendEndOfInterrupt(int p_IRQ);

} // namespace Intel386
} // namespace Nutshell

#endif // !INTEL386_INTERRUPTS_H
