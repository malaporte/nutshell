//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_H
#define INTEL386_H

#include "Intel386/GlobalDescriptorTable.h"
#include "Intel386/InterruptDescriptorTable.h"
#include "Intel386/PageDirectory.h"
#include "Intel386/TaskStateSegment.h"

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// Misc. machine related constants.
//******************************************************************************

// Constants on how are organized entries in the GDT
const size_t KERNEL_CODE_SEGMENT            = 1;
const size_t KERNEL_DATA_SEGMENT            = 2;
const size_t USER_CODE_SEGMENT              = 3;
const size_t USER_DATA_SEGMENT              = 4;
const size_t FIRST_TSS_SEGMENT              = 5;

// PIC related constants
const size_t MASTER_PIC_BASE_PORT           = 0x20;
const size_t SLAVE_PIC_BASE_PORT            = 0xA0;
const size_t MASTER_PIC_BASE_VECTOR         = 32;
const size_t SLAVE_PIC_BASE_VECTOR          = 40;
const size_t MASTER_SLAVE_IRQ               = 2;

// RTC related constants
const size_t RTC_BASE_PORT                  = 0x70;
const size_t RTC_INTERRUPT                  = 8;

// Kernel debugging related constants
const size_t DEBUGGING_BASE_PORT            = 0x3F8;

extern GlobalDescriptorTable*               g_pGDT;                 // The global descriptor table.
extern InterruptDescriptorTable*            g_pIDT;                 // The interrupt descriptor table.
extern PageDirectory*                       g_pPD;                  // The initial page directory.
extern TaskStateSegment*                    g_pTSS;                 // The initial task state segment.
extern unsigned                             g_MemorySize;           // The size of the memory, in bytes.
extern char                                 g_EmergencyStack[4096]; // The stack used for booting and emergencies.

//******************************************************************************
// Misc. machine related functions.
//******************************************************************************

void    InitializeMachine();
void    OutPort(int p_Port, char p_Value);
char    InPort(int p_Port);

} // namespace Intel386
} // namespace Nutshell

#endif // !INTEL386_H
