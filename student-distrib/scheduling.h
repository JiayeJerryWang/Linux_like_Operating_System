#ifndef SCHDULE
#define SCHDULE

#include "lib.h"
#include "i8259.h"
#include "lib.h"
#include "system_call.h"
#include "terminal.h"

// PIT functions declared here, see more details in scheduling.c
void init_PIT();
// PIT handler
void PIT_handler();

#endif
