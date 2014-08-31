//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"

#include <pthread.h>
#include <boost/static_assert.hpp>

namespace Nutshell {
namespace Utilities {

#define NOT_IMPLEMENTED_FUNCTION(name) extern "C" void name() { PANIC(#name " not implemented."); }

NOT_IMPLEMENTED_FUNCTION(sysconf);
NOT_IMPLEMENTED_FUNCTION(pthread_create);
NOT_IMPLEMENTED_FUNCTION(pthread_once);
NOT_IMPLEMENTED_FUNCTION(pthread_atfork);
NOT_IMPLEMENTED_FUNCTION(pthread_key_create);
NOT_IMPLEMENTED_FUNCTION(pthread_setspecific);
NOT_IMPLEMENTED_FUNCTION(pthread_getspecific);
NOT_IMPLEMENTED_FUNCTION(pthread_mutex_lock);
NOT_IMPLEMENTED_FUNCTION(pthread_mutex_unlock);
NOT_IMPLEMENTED_FUNCTION(GetAtomNameA);
NOT_IMPLEMENTED_FUNCTION(AddAtomA);
NOT_IMPLEMENTED_FUNCTION(FindAtomA);

NOT_IMPLEMENTED_FUNCTION(_Unwind_Register);
NOT_IMPLEMENTED_FUNCTION(_Unwind_Unregister);
NOT_IMPLEMENTED_FUNCTION(_Unwind_Resume);
NOT_IMPLEMENTED_FUNCTION(__cxa_allocate_exception);
NOT_IMPLEMENTED_FUNCTION(__cxa_throw);
NOT_IMPLEMENTED_FUNCTION(__cxa_begin_catch);
NOT_IMPLEMENTED_FUNCTION(__cxa_rethrow);
NOT_IMPLEMENTED_FUNCTION(__cxa_end_catch);
NOT_IMPLEMENTED_FUNCTION(__cxa_pure_virtual);

NOT_IMPLEMENTED_FUNCTION(__gxx_personality_sj0);

NOT_IMPLEMENTED_FUNCTION(__assert_func);
NOT_IMPLEMENTED_FUNCTION(__getreent);
NOT_IMPLEMENTED_FUNCTION(fprintf);
NOT_IMPLEMENTED_FUNCTION(_pei386_runtime_relocator);
NOT_IMPLEMENTED_FUNCTION(time);

} // namespace Utilities
} // namespace Nutshell
