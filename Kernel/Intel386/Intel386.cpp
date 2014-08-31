//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/Intel386.h"
#include "Intel386/Interrupts.h"
#include "Intel386/Handlers.h"
#include "Intel386/Paging.h"

namespace Nutshell {
namespace Intel386 {

// Variables defined in the header
GlobalDescriptorTable*      g_pGDT;
InterruptDescriptorTable*   g_pIDT;
PageDirectory*              g_pPD;
TaskStateSegment*           g_pTSS;
unsigned                    g_MemorySize;
char                        g_EmergencyStack[4096];

//******************************************************************************
// This is the first thing called when the kernel starts.
//
// At this point, we  got  a completely valid working  environment, including a
// stack, place for  a heap,  etc.  We will  still  need  to  re-establish this
// environment  before  we  can  begin  to  play with  memory  outside  what we
// initially have, in  order not to overwrite whatever CPU structures  that the
// bootstrap may have placed about everywhere in memory.
//
// We will then call the global kernel entry point.
//******************************************************************************
extern "C" void MachineStartup()
{
    // TODO: Those should  get filled with kernel parameters,  once they become
    // available. For now, the settings are static.
    bool kernelDebugging = true;

    //--------------------------------------------------------------------------
    // Standard C++ specifies that the  BSS section should be zeroed, so do it
    // now. Note that the malloc we're using depends on this...
    //--------------------------------------------------------------------------

    memset(&_BEGINNING_OF_BSS, 0, &_END_OF_BSS - &_BEGINNING_OF_BSS);

    //--------------------------------------------------------------------------
    // Probe the size of physical memory.
    //--------------------------------------------------------------------------

    // TODO: This is broken, because of early paging enable, so fix it!
    g_MemorySize = 16 * 1024 * 1024;
/*
    asm(
        // Begin probing at the end of the kernel
        "       mov     %1, %%eax ;"

        // around (in the case where the system has 4gb of memory !)
        "0:     addl    $0x100000, %%eax ;"
        "       cmpl    %1, %%eax ;"
        "       jle     1f ;"

        // Write  at the  memory  and check  the result  to see  if was really
        // written
        "       movl    (%%eax), %%ebx ;"
        "       movl    $0x12345678, (%%eax) ;"
        "       wbinvd ;"
        "       cmpl    $0x12345678, (%%eax) ;"
        "       movl    %%ebx, (%%eax) ;"
        "       je      0b ;"

        // Put the result in the output variable
        "1:     movl    %%eax, %0 ;"

        : "=g" (g_MemorySize)
        : "g" (&_END_OF_BSS)
        : "eax", "ebx", "flags"
    );

    // Round the value that was returned to megabytes
    g_MemorySize -= g_MemorySize % 0x100000;
*/
    //--------------------------------------------------------------------------
    // Setup the global descriptor table.
    //
    // We  create two code and  data  segments, so that  we have privileged and
    // non-privileged segments suitable for kernel and user code.
    //
    // We do  not further use the  descriptor table features of  the processor,
    // as we implement protection using virtual memory.
    //--------------------------------------------------------------------------

    // Create the global descriptor table
    PANIC("poo");
    g_pGDT = new GlobalDescriptorTable();


    // Setup the kernel code segment
    (*g_pGDT)[KERNEL_CODE_SEGMENT].Base(0);
    (*g_pGDT)[KERNEL_CODE_SEGMENT].Limit(0xFFFFF);
    (*g_pGDT)[KERNEL_CODE_SEGMENT].G(true);
    (*g_pGDT)[KERNEL_CODE_SEGMENT].DB(true);
    (*g_pGDT)[KERNEL_CODE_SEGMENT].P(true);
    (*g_pGDT)[KERNEL_CODE_SEGMENT].S(true);
    (*g_pGDT)[KERNEL_CODE_SEGMENT].Type(SegmentDescriptor::ExecuteOnly);

    // Setup the kernel data segment
    (*g_pGDT)[KERNEL_DATA_SEGMENT].Base(0);
    (*g_pGDT)[KERNEL_DATA_SEGMENT].Limit(0xFFFFF);
    (*g_pGDT)[KERNEL_DATA_SEGMENT].G(true);
    (*g_pGDT)[KERNEL_DATA_SEGMENT].DB(true);
    (*g_pGDT)[KERNEL_DATA_SEGMENT].P(true);
    (*g_pGDT)[KERNEL_DATA_SEGMENT].S(true);
    (*g_pGDT)[KERNEL_DATA_SEGMENT].Type(SegmentDescriptor::ReadWrite);

    // Setup the user code segment
    (*g_pGDT)[USER_CODE_SEGMENT].Base(0);
    (*g_pGDT)[USER_CODE_SEGMENT].Limit(0xFFFFF);
    (*g_pGDT)[USER_CODE_SEGMENT].G(true);
    (*g_pGDT)[USER_CODE_SEGMENT].DB(true);
    (*g_pGDT)[USER_CODE_SEGMENT].P(true);
    (*g_pGDT)[USER_CODE_SEGMENT].DPL(3);
    (*g_pGDT)[USER_CODE_SEGMENT].S(true);
    (*g_pGDT)[USER_CODE_SEGMENT].Type(SegmentDescriptor::ExecuteOnly);

    // Setup the user data segment
    (*g_pGDT)[USER_DATA_SEGMENT].Base(0);
    (*g_pGDT)[USER_DATA_SEGMENT].Limit(0xFFFFF);
    (*g_pGDT)[USER_DATA_SEGMENT].G(true);
    (*g_pGDT)[USER_DATA_SEGMENT].DB(true);
    (*g_pGDT)[USER_DATA_SEGMENT].P(true);
    (*g_pGDT)[USER_DATA_SEGMENT].DPL(3);
    (*g_pGDT)[USER_DATA_SEGMENT].S(true);
    (*g_pGDT)[USER_DATA_SEGMENT].Type(SegmentDescriptor::ReadWrite);

    // Declare and fill a structure that will be used to call LGDT
    struct {
        unsigned short  g_Limit __attribute__((packed));
        unsigned        g_Address __attribute__((packed));
    } lgdtParams;
    lgdtParams.g_Limit = sizeof(GlobalDescriptorTable) - 1;
    lgdtParams.g_Address = reinterpret_cast<unsigned>(g_pGDT);

    // Call LGDT and load segment registers with correct values
    asm("lgdtl %0" : : "m" (lgdtParams));
    asm("movw   %0, %%ds" : : "r" (static_cast<short>(KERNEL_DATA_SEGMENT << 3)));
    asm("movw   %0, %%es" : : "r" (static_cast<short>(0)));
    asm("movw   %0, %%fs" : : "r" (static_cast<short>(0)));
    asm("movw   %0, %%gs" : : "r" (static_cast<short>(0)));

    // Declare and fill a structure that will be used to call LJMP
    //
    // NOTE:  The offset  part  will  be filled  by  the  assembler code to the
    // address of a local label defined within it.
    struct {
        unsigned        g_Offset   __attribute__((packed));
        unsigned short  g_Selector __attribute__((packed));
    } ljmpParams;
    ljmpParams.g_Selector = KERNEL_CODE_SEGMENT << 3;

    // Perform a jump to the kernel code segment
    asm("movl $0f, %1 ; ljmp *%0 ; 0:" : : "m" (ljmpParams), "m" (ljmpParams.g_Offset));
                                                
    //--------------------------------------------------------------------------
    // Setup the interrupt descriptor table.
    //--------------------------------------------------------------------------

    // Create the interrupt descriptor table
    g_pIDT = new InterruptDescriptorTable();

    // Declare and fill a structure that will be used to call LIDT
    struct {
        unsigned short  g_Limit __attribute__((packed));
        unsigned        g_Address __attribute__((packed));
    } lidtParams;
    lidtParams.g_Limit = sizeof(InterruptDescriptorTable);
    lidtParams.g_Address = reinterpret_cast<unsigned>(g_pIDT);

    // Call LIDT
    asm("lidtl %0" : : "m" (lidtParams));

    // Install the various system interrupt handlers
/*
    InstallInterruptHandler(0, &DivideErrorInterruptHandler, false);
    InstallInterruptHandler(1, &DebugInterruptHandler, false);
    InstallInterruptHandler(3, &BreakpointInterruptHandler, false);
    InstallInterruptHandler(4, &OverflowInterruptHandler, false);
    InstallInterruptHandler(5, &BoundRangeExceededInterruptHandler, false);
    InstallInterruptHandler(6, &InvalidOpcodeInterruptHandler, false);
    InstallInterruptHandler(7, &DeviceNotAvailableInterruptHandler, false);
    InstallInterruptHandler(8, &DoubleFaultInterruptHandler, true);
    InstallInterruptHandler(9, &CoprocessorSegmentOverrunInterruptHandler, false);
    InstallInterruptHandler(10, &InvalidTSSInterruptHandler, true);
    InstallInterruptHandler(11, &SegmentNotPresentInterruptHandler, true);
    InstallInterruptHandler(12, &StackSegmentFaultInterruptHandler, true);
    InstallInterruptHandler(13, &GeneralProtectionInterruptHandler, true);
    InstallInterruptHandler(14, &PageFaultInterruptHandler, true);
    InstallInterruptHandler(16, &FloatingPointErrorInterruptHandler, false);
    InstallInterruptHandler(17, &AlignmentCheckInterruptHandler, true);
    InstallInterruptHandler(18, &MachineCheckInterruptHandler, false);
    InstallInterruptHandler(19, &StreamingSIMDInterruptHandler, false);
*/
    InstallHardwareInterruptHandler(RTC_INTERRUPT, &ClockInterruptHandler);

    //--------------------------------------------------------------------------
    // Initialize the Programmable Interrupt Controller.
    //--------------------------------------------------------------------------

    // Send the ICW1 to both master and slave
    OutPort(MASTER_PIC_BASE_PORT,       (1 << 4) | (1 << 0));
    OutPort(SLAVE_PIC_BASE_PORT,        (1 << 4) | (1 << 0));

    // Send the ICW2 to both master and slave
    OutPort(MASTER_PIC_BASE_PORT + 1,   MASTER_PIC_BASE_VECTOR);
    OutPort(SLAVE_PIC_BASE_PORT + 1,    SLAVE_PIC_BASE_VECTOR);

    // Send the ICW3 to both master and slave
    OutPort(MASTER_PIC_BASE_PORT + 1,   1 << MASTER_SLAVE_IRQ);
    OutPort(SLAVE_PIC_BASE_PORT + 1,    MASTER_SLAVE_IRQ);

    // Send the ICW4 to both master and slave
    OutPort(MASTER_PIC_BASE_PORT + 1,   1);
    OutPort(SLAVE_PIC_BASE_PORT + 1,    1);

    // Mask all interrupts except the one used to cascade
    SetInterruptMask(0xFFFF & ~(1 << MASTER_SLAVE_IRQ));

    //--------------------------------------------------------------------------
    // Initialize the serial port used for kernel debugging.
    //--------------------------------------------------------------------------

    // Initialize only if kernel debugging is enabled
    if (kernelDebugging) {
        // Set the port's speed (9600 bauds)
        OutPort(DEBUGGING_BASE_PORT + 3, 0x80);
        OutPort(DEBUGGING_BASE_PORT + 0, 0x0C);
        OutPort(DEBUGGING_BASE_PORT + 1, 0x00);

        // Set 8 bits, no parity, 1 stop bit mode
        OutPort(DEBUGGING_BASE_PORT + 3, 0x03);

        // Initialize the FIFO control register
        OutPort(DEBUGGING_BASE_PORT + 2, 0x07);

        // Initialize the modem control register
        OutPort(DEBUGGING_BASE_PORT + 4, 0x03);
    }

    //--------------------------------------------------------------------------
    // Initialize the Real Time Clock.
    //
    // This interrupt is called at regular  intervals and  is used to implement
    // preemptive multitasking.
    //
    // The  interrupt  won't be raised until we  re-enable interrupts  with the
    // processor (this is done when the basic kernel is done initializing).
    //--------------------------------------------------------------------------

    // Unmask the clock interrupt
    SetInterruptMask(GetInterruptMask() & ~(1 << RTC_INTERRUPT));

    // Setup periodic interrupt frequency (256 ints per second)
    // TODO: Integrate with CLOCKS_PER_SECOND ...
    OutPort(RTC_BASE_PORT,        0xA);
    OutPort(RTC_BASE_PORT + 1,    (1 << 5) | 2);

    // Tell the RTC that we want periodic interrupts
    OutPort(RTC_BASE_PORT,        0xB);
    char status = InPort(RTC_BASE_PORT + 1);
    OutPort(RTC_BASE_PORT,        0xB);
    OutPort(RTC_BASE_PORT + 1,    status | (1 << 6));

    //--------------------------------------------------------------------------
    // Initialize the first page directory.
    //--------------------------------------------------------------------------

    // Create the first  page directory that will replace the current one that
    // comes from  the bootstrap.  It  will  really do the same  thing, except
    // that we won't have to bother about  overwriting it since it will reside
    // in the kernel address space.
    //
    // This  page directory will get replaced  on  the first switch to a 'real'
    // task, but we  will keep it since it's  handy to have a page directory in
    // case of panic.
    g_pPD = new (memalign(OS_PAGE_SIZE, sizeof(PageDirectory))) PageDirectory();

    // Create the first page table that maps the first 4mb of physical memory.
    //
    // This is needed  in  order to be able to access system stuff that resides
    // there, like the screen buffer, and so on.
    PageTable* pTable = new (memalign(OS_PAGE_SIZE, sizeof(PageTable))) PageTable();

    // Setup all entries within the page table
    for (size_t i = 0; i < PAGE_TABLE_CAPACITY; ++i) {
        (*pTable)[i].Base(i * OS_PAGE_SIZE);
        (*pTable)[i].P(true);
        (*pTable)[i].RW(true);
    }

    // Map the first page within the page directory
    (*g_pPD)[0].Base(GetPhysicalAddress(pTable));
    (*g_pPD)[0].P(true);
    (*g_pPD)[0].RW(true);

    // Now  create  the page tables that will hold the kernel memory until the
    // pager is up. We create enough of them for a 60mb kernel.

    // Create the page tables for the kernel
    for (size_t i = 0; i < 15; ++i) {
        // Create the next page table
        PageTable* pTable = new (memalign(OS_PAGE_SIZE, sizeof(PageTable))) PageTable();

        // Setup all entries within the page table
        for (size_t j = 0; j < PAGE_TABLE_CAPACITY; ++j) {
            (*pTable)[j].Base(GetPhysicalAddress(reinterpret_cast<void*>(KERNEL_SPACE_BOUNDARY + i * PAGE_TABLE_SIZE + j * OS_PAGE_SIZE)));
            (*pTable)[j].P(true);
            (*pTable)[j].RW(true);
        }

        // Add it to the page directory
        (*g_pPD)[KERNEL_SPACE_BOUNDARY / PAGE_TABLE_SIZE + i].Base(GetPhysicalAddress(pTable));
        (*g_pPD)[KERNEL_SPACE_BOUNDARY / PAGE_TABLE_SIZE + i].P(true);
        (*g_pPD)[KERNEL_SPACE_BOUNDARY / PAGE_TABLE_SIZE + i].RW(true);
    }

    // Load the page directory we created
    asm("movl %0, %%cr3" : : "r" (GetPhysicalAddress(g_pPD)));

    //--------------------------------------------------------------------------
    // Initialize the first task selector.
    //--------------------------------------------------------------------------

    // The first task selector is  the  one we'll depart  from when we'll make
    // the first task switch,  and  it needs to  be initialized,  so  we do it
    // now. First we must allocate a task state segment.
    g_pTSS = new TaskStateSegment();

    // Make the first TSS descriptor point to the memory we allocated
    (*g_pGDT)[FIRST_TSS_SEGMENT].Base(reinterpret_cast<unsigned>(g_pTSS));
    (*g_pGDT)[FIRST_TSS_SEGMENT].Limit(sizeof(TaskStateSegment) - 1);
    (*g_pGDT)[FIRST_TSS_SEGMENT].G(true);
    (*g_pGDT)[FIRST_TSS_SEGMENT].P(true);
    (*g_pGDT)[FIRST_TSS_SEGMENT].Type(SegmentDescriptor::TSS32BitAvailable);

    // Load  the task register with the selector for the TSS we allocated
    unsigned short selector = FIRST_TSS_SEGMENT << 3;
    asm("ltr %0" : : "m" (selector));

    //--------------------------------------------------------------------------
    // Initialization is done, we may call the global kernel entry point.
    //--------------------------------------------------------------------------

    asm(
        // Position the stack pointer in the emergency stack
        "movw   %0, %%ss ;"
        "movl   %1, %%esp ;"

        // Call the global kernel entry point
        "jmp    _start ;"
        
        :
        :   "r" (static_cast<short>(KERNEL_DATA_SEGMENT << 3)),
            "r" (g_EmergencyStack + sizeof(g_EmergencyStack))
    );
}

//******************************************************************************
// Writes a value to a port.
//
// Parameters:
//  p_Port  - The port to which to send to.
//  p_Value - The value to send to the port.
//******************************************************************************
void OutPort(int p_Port, char p_Value)
{
    assert(p_Port >= 0);
    assert(p_Port < 65536);

    // Send the value to the port
    asm("outb %1, %0" : : "d" (static_cast<short>(p_Port)), "a" (p_Value));
}

//******************************************************************************
// Reads a value from a port.
//
// Parameters:
//  p_Port  - The port to which to send to.
//
// Returns:
//  The value that was read.
//******************************************************************************
char InPort(int p_Port)
{
    assert(p_Port >= 0);
    assert(p_Port < 65536);

    // Retrieve a value from the port
    char value;
    asm("inb %1, %0" : "=a" (value) : "d" (static_cast<short>(p_Port)));

    return value;
}

} // namespace Intel386
} // namespace Nutshell
