//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/BIOS.h"
#include "Intel386/Intel386.h"

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// Calls the BIOS using a specific context.
//
// Parameters:
//  p_Vector - The interrupt vector to call.
//******************************************************************************
void CallBIOS(int p_Vector)
{
    assert(p_Vector >= 0);
    assert(p_Vector < 256);
}

} // namespace Intel386
} // namespace Nutshell

