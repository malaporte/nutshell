//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef THREADING_PROCESS_H
#define THREADING_PROCESS_H

#include "Paging/Pager.h"

namespace Nutshell {
namespace Threading {

//******************************************************************************
// This class encapsulates a process.
//******************************************************************************
class Process
:   public Paging::Pageable,
    boost::noncopyable
{
private:

public:

    // Construction / destruction
    Process();
    ~Process();
};

typedef boost::shared_ptr<Process> ProcessSP;
typedef boost::weak_ptr<Process> ProcessWP;

} // namespace Threading
} // namespace Nutshell

#endif // !THREADING_PROCESS_H
