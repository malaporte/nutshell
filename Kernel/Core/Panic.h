//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef CORE_PANIC_H
#define CORE_PANIC_H

namespace Nutshell {
namespace Core {

// Panic functions
void Panic(const char* p_pMessage, const char* p_pFile, int p_Line);

} // namespace Core
} // namespace Nutshell

#endif // !CORE_PANIC_H
