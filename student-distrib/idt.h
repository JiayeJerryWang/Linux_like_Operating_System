/* idt.h - Defines for idt.c
 * vim:ts=4 noexpandtab
 */
#include "x86_desc.h"
#include "lib.h"
#include "types.h"
#include "rtc.h"
#include "linkage.h"
#include "keyboard.h"
#include "i8259.h"
#include "system_call_linkage.h"


#ifndef _IDT_H
#define _IDT_H

/* initialize the idt */
extern void Initilize_IDT_Table();

/* exception handler */
extern void Divide_Error_Exception();

/* exception handler */
extern void RESERVED_Exception();

/* exception handler */
extern void NMI_Interrupt_Exception();

/* exception handler */
extern void Breakpoint_Exception();

/* exception handler */
extern void Overflow_Exception();

/* exception handler */
extern void BOUND_Range_Exceeded_Exception();

/* exception handler */
extern void Invalid_Opcode_Exception();

/* exception handler */
extern void Device_Not_Available_Exception();

/* exception handler */
extern void Double_Fault_Exception();

/* exception handler */
extern void Coprocessor_Segment_Overrun_Exception();

/* exception handler */
extern void Invalid_TSS_Exception();

/* exception handler */
extern void Segment_Not_Present_Exception();

/* exception handler */
extern void Stack_Segment_Fault_Exception();

/* exception handler */
extern void General_Protection_Exception();

/* exception handler */
extern void Page_Fault_Exception();

/* exception handler */
extern void Math_Fault_Exception();

/* exception handler */
extern void Alignment_Check_Exception();

/* exception handler */
extern void Machine_Check_Exception();

/* exception handler */
extern void SIMD_Floating_Point_Exception();

/* exception handler */
extern void Intel_reserved_Exception();

#endif /* _IDT_H */
