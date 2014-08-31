//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

// This file is just a  wrapper  for the  real  architecture dependent machine
// include file.

#ifndef MACHINE_H
#define MACHINE_H

#ifdef _INTEL386_
    #include "Intel386/Intel386.h"
    #include "Intel386/Panic.h"
    #include "Intel386/Paging.h"
    #include "Intel386/Tasking.h"
    #include "Intel386/Interrupts.h"
#else
    #error "Include the proper machine header here..."
#endif

namespace Nutshell {
namespace Machine {

// Import the machine API within this namespace
#ifdef _INTEL386_
using namespace Intel386;
#else
    #error "Import the proper namespace here..."
#endif

} // namespace Machine
} // namespace Nutshell

#endif // !MACHINE_H
