//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_PANIC_H
#define INTEL386_PANIC_H

namespace Nutshell {
namespace Intel386 {

// Panic functions
void Panic(const char* p_pMessage, const char* p_pFile, int p_Line);

} // namespace Intel386
} // namespace Nutshell

#endif // !PANIC_H
