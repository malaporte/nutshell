//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/Interrupts.h"
#include "Intel386/Intel386.h"

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// This  is the code that  is used to wrap interruptions. It is  designed to be
// copied  into an  allocated  buffer and then modified  so as to customize the
// address of the handler function.
//******************************************************************************
asm (
    // This is the entry point of the wrapper
    "_intel386_interrupt_wrapper: ;"

    // TODO:  The interrupt  handler should also  maybe  preserve floating
    // point registers, but I'm not sure yet...

    // Preserve registers that aren't restored by iret
    "pushl  %eax ;"
    "pushl  %ebx ;"
    "pushl  %ecx ;"
    "pushl  %edx ;"

    // The value here will get replaced with the address of the handler.
    "movl   $0xABCD1234, %eax ;"

    // Call a dummy handler function. The address will get replaced.
    //
    // Arguments to it are :
    //  - the value of %eip before the interrupt.
    //  - the optional exception parameter.
    //
    // The exception parameter 
    "pushl  $0 ;"
    "pushl  20(%esp) ;"
    "call   *%eax ;"
    "addl   $8, %esp ;"

    // Restore preserved registers.
    "popl   %edx ;"
    "popl   %ecx ;"
    "popl   %ebx ;"
    "popl   %eax ;"

    // Return from the interrupt
    "iret ;"

    // Used to compute the size of the wrapper
    "_intel386_end_of_interrupt_wrapper: ;"
);

extern "C" {
    extern char intel386_interrupt_wrapper;
    extern char intel386_end_of_interrupt_wrapper;
}

//******************************************************************************
// Installs a new interrupt handler.
//
// Parameters:
//  p_Vector    - The interrupt vector.
//  p_pFunction - The function to call when an interrupt occurs.
//  p_Parameter - Whether the interrupt pushes a parameter on the stack.
//******************************************************************************
void InstallInterruptHandler(int p_Vector, void (*p_pFunction)(void*, void*), bool p_Parameter)
{
    assert(p_Vector >= 0);
    assert(p_Vector < 256);
    assert(p_pFunction != 0);

    //
    // We create  an  assembler  wrapper  that  calls the  given function. This
    // function  has  two  parameters  :  p_pEIP and  p_pParam,  which contains
    // information about the exception.
    //

    // Retrieve pointers to the beginning end the end of the wrapper
    char* begin = &intel386_interrupt_wrapper;
    char* end = &intel386_end_of_interrupt_wrapper;
    assert(begin < end);

    // Allocate memory for the handler, and copy the code in it
    char* pHandler = new char[end - begin];
    memcpy(pHandler, begin, end - begin);
    end = pHandler + (end - begin);
    begin = pHandler;

    // Replace the dummy handler address with a real one
    //
    // TODO: Ensure the code doesn't contain 2 dummy values!
    size_t searched = 0xABCD1234;
    char* found = std::search(begin,
                              end,
                              reinterpret_cast<char*>(&searched),
                              reinterpret_cast<char*>(&searched + 1));
    assert(found != end);
    *reinterpret_cast<size_t*>(found) = reinterpret_cast<size_t>(p_pFunction);

    // Set up the interrupt vector
    InstallRawInterruptHandler(p_Vector, pHandler);
}

//******************************************************************************
// Installs a raw interrupt handler.
//
// Parameters:
//  p_Vector    - The interrupt vector.
//  p_pHandler  - The address of the raw interrupt handler.
//******************************************************************************
void InstallRawInterruptHandler(int p_Vector, void* p_pHandler)
{
    assert(p_Vector >= 0);
    assert(p_Vector < 256);
    assert(!(*g_pIDT)[p_Vector].P());
    assert(p_pHandler != 0);

    // Set up the interrupt vector
    (*g_pIDT)[p_Vector].Segment(KERNEL_CODE_SEGMENT << 3);
    (*g_pIDT)[p_Vector].Offset(reinterpret_cast<unsigned>(p_pHandler));
    (*g_pIDT)[p_Vector].P(true);
    (*g_pIDT)[p_Vector].D(true);
    (*g_pIDT)[p_Vector].Type(GateDescriptor::InterruptGate);
}

//******************************************************************************
// Removes an interrupt handler.
//
// Parameters:
//  p_Vector    - The interrupt vector.
//******************************************************************************
void UninstallInterruptHandler(int p_Vector)
{
    assert(p_Vector >= 0);
    assert(p_Vector < 256);
    assert((*g_pIDT)[p_Vector].P());

    // Mark the interrupt vector as not present
    (*g_pIDT)[p_Vector].P(false);

    // Release the memory that contains the interrupt vector code
    free(reinterpret_cast<void*>((*g_pIDT)[p_Vector].Offset()));
}

//******************************************************************************
// Installs a new hardware interrupt handler.
//
// Parameters:
//  p_Interrupt - The hardware interrupt.
//  p_pFunction - The function to call when an interrupt occurs.
//******************************************************************************
void InstallHardwareInterruptHandler(int p_Interrupt, void (*p_pFunction)(void*, void*))
{
    assert(p_Interrupt >= 0);
    assert(p_Interrupt < 16);
    assert(p_pFunction != 0);

    // Check which PIC controls the given interrupt
    if (p_Interrupt < 8) {
        InstallInterruptHandler(MASTER_PIC_BASE_VECTOR + p_Interrupt, p_pFunction, false);
    } else {
        InstallInterruptHandler(SLAVE_PIC_BASE_VECTOR + p_Interrupt - 8, p_pFunction, false);
    }
}

//******************************************************************************
// Allows interrupts to be generated.
//******************************************************************************
void EnableInterrupts()
{
    // Enable interrupts
    asm("sti");
}

//******************************************************************************
// Prevents interrupts from being generated.
//
// Returns:
//  Whether interrupts were previously enabled.
//******************************************************************************
bool DisableInterrupts()
{
    // Retrieve the content of the /eflags/ register
    unsigned eflags;
    asm("pushfl ; popl %0" : "=r" (eflags));

    // Disable interrupts
    asm("cli");

    return Utilities::BitTest(eflags, 9);
}

//******************************************************************************
// Retrieves the the interrupt mask.
//
// Returns:
//  The interrupt mask.
//******************************************************************************
unsigned short GetInterruptMask()
{
    // Read new interrupt mask from both PIC's
    unsigned short mask = 0;
    mask |= InPort(MASTER_PIC_BASE_PORT + 1);
    mask |= InPort(SLAVE_PIC_BASE_PORT + 1) << 8;

    return mask;
}

//******************************************************************************
// Sets the interrupt mask.
//
// Parameters:
//  p_Mask - The new interrupt mask.
//******************************************************************************
void SetInterruptMask(unsigned short p_Mask)
{
    // Send the new interrupt mask to both PIC's
    OutPort(MASTER_PIC_BASE_PORT + 1,   p_Mask & 0x00FF);
    OutPort(SLAVE_PIC_BASE_PORT + 1,    (p_Mask & 0xFF00) >> 8);
}

//******************************************************************************
// Sends an End Of Interrupt signal to the PIC.
//
// Parameters:
//  p_IRQ - The IRQ for which the EOI must be sent.
//******************************************************************************
void SendEndOfInterrupt(int p_IRQ)
{
    assert(p_IRQ >= 0);
    assert(p_IRQ < 16);

    // Send the EOI to the slave PIC if needed
    if (p_IRQ >= 8) {
        OutPort(SLAVE_PIC_BASE_PORT, 1 << 5);
    }

    // Send the EOI to the master PIC
    OutPort(MASTER_PIC_BASE_PORT, 1 << 5);
}

} // namespace Intel386
} // namespace Nutshell

