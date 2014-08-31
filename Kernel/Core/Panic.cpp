//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Machine.h"
#include "Core/Panic.h"

namespace Nutshell {
namespace Core {

//******************************************************************************
// Handles a kernel panic (displays an error message and stop).
//
// Parameters:
//  p_pMessage  - The message associated with the panic.
//  p_pFile     - The file in which the panic occured.
//  p_Line      - The line number at which the panic occured.
//******************************************************************************
void Panic(const char* p_pMessage, const char* p_pFile, int p_Line)
{
    // Panic using machine specific handler
    Machine::Panic(p_pMessage, p_pFile, p_Line);
}

//******************************************************************************
// Forward uses of the assert function to Panic.
//
// Parameters:
//  p_pTest - The test that failed.
//  p_pFile - The file where the test failed.
//  p_Line  - The line where the test failed.
//******************************************************************************
extern "C" void __assert(const char* p_pTest, const char* p_pFile, int p_Line)
{
    Panic(p_pTest, p_pFile, p_Line);
}

//******************************************************************************
// Forward uses of the write function to Panic.
//
// Parameters:
//  p_File    - File descriptor. Unused.
//  p_pBuffer - Buffer holding what we should write.
//  p_Count   - Number of bytes to write.
//
// Returns:
//  Number of bytes actually written.
//******************************************************************************
extern "C" int write(int p_File, const void* p_pBuffer, unsigned int count)
{
    Panic("Use of the write() function. Probably a pure virtual call.", __FILE__, __LINE__);
}

} // namespace Core
} // namespace Nutshell
