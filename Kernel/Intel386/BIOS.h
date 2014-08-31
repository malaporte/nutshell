//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_BIOS_H
#define INTEL386_BIOS_H

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// BIOS interface functions.
//******************************************************************************

void            CallBIOS(int p_Vector);

} // namespace Intel386
} // namespace Nutshell

#endif // !INTEL386_BIOS_H
