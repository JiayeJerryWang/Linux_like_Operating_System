#ifndef _system_call_linkage_h
#define _system_call_linkage_h
#ifndef asm

#include "system_call.h"
#include "types.h"

// the system call linkage function, check more details in system_call_linkage.S
void system_call_handler (void);

#endif
#endif
