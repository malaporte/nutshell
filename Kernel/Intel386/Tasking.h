//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef INTEL386_TASKING_H
#define INTEL386_TASKING_H

namespace Nutshell {
namespace Intel386 {

//******************************************************************************
// Task management functions.
//******************************************************************************

int     AllocateTaskDescriptor(size_t p_Directory, size_t p_Stack, void (* p_pEntry)(void*));
void    ReleaseTaskDescriptor(int p_Task);
int     GetCurrentTaskDescriptor();
void    SwitchToTaskDescriptor(int p_Task);

} // namespace Intel386
} // namespace Nutshell

#endif // !INTEL386_TASKING_H
