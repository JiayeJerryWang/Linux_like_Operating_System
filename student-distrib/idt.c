/* idt.c - initialize the idt 
 * vim:ts=4 noexpandtab */

#include "idt.h"
/* void Initilize_IDT_Table();
 * Inputs: void
 * Return Value: none
 * Function:  initialize the idt table */
void Initilize_IDT_Table() {
    int i;
    for (i = 0; i < NUM_VEC; i++) {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 1 ;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        idt[i].present = 1;
        /* reserved3 bit set to 0 to set interrupt gate */
        if ((i == 2) | (i >= 32)) {
            idt[i].reserved3 = 0;
        }
        if ((i == 128)) {
            idt[i].reserved3 = 1;
            idt[i].dpl = 3;
        }
    }

    /* set the idt entry for specfic faults and interrupts */
    SET_IDT_ENTRY(idt[0], &Divide_Error_Exception);
    SET_IDT_ENTRY(idt[1], &RESERVED_Exception);
    SET_IDT_ENTRY(idt[2], &NMI_Interrupt_Exception);
    SET_IDT_ENTRY(idt[3], &Breakpoint_Exception);
    SET_IDT_ENTRY(idt[4], &Overflow_Exception);
    SET_IDT_ENTRY(idt[5], &BOUND_Range_Exceeded_Exception);
    SET_IDT_ENTRY(idt[6], &Invalid_Opcode_Exception);
    SET_IDT_ENTRY(idt[7], &Device_Not_Available_Exception);
    SET_IDT_ENTRY(idt[8], &Double_Fault_Exception);
    SET_IDT_ENTRY(idt[9], &Coprocessor_Segment_Overrun_Exception);
    SET_IDT_ENTRY(idt[10], &Invalid_TSS_Exception);
    SET_IDT_ENTRY(idt[11], &Segment_Not_Present_Exception);
    SET_IDT_ENTRY(idt[12], &Stack_Segment_Fault_Exception);
    SET_IDT_ENTRY(idt[13], &General_Protection_Exception);
    SET_IDT_ENTRY(idt[14], &Page_Fault_Exception);
    SET_IDT_ENTRY(idt[15], &Intel_reserved_Exception); // reserved by intel
    SET_IDT_ENTRY(idt[16], &Math_Fault_Exception);
    SET_IDT_ENTRY(idt[17], &Alignment_Check_Exception);
    SET_IDT_ENTRY(idt[18], &Machine_Check_Exception);
    SET_IDT_ENTRY(idt[19], &SIMD_Floating_Point_Exception);
    SET_IDT_ENTRY(idt[32], &PIT_linkage);               //PIT likage
    SET_IDT_ENTRY(idt[33], &keyboard_linkage);
    SET_IDT_ENTRY(idt[40], &rtc_linkage);
    SET_IDT_ENTRY(idt[128], &system_call_handler);
}

/* exception handler */
void Divide_Error_Exception() {
    //clear();
    printf("\nDivide_Error_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void RESERVED_Exception() {
    //clear();
    printf("\nRESERVED_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void NMI_Interrupt_Exception() {
    //clear();
    printf("\nNMI_Interrupt_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Breakpoint_Exception() {
    //clear();
    printf("\nBreakpoint_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Overflow_Exception() {
    //clear();
    printf("\nOverflow_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void BOUND_Range_Exceeded_Exception() {
    //clear();
    printf("\nBOUND_Range_Exceeded_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Invalid_Opcode_Exception() {
    //clear();
    printf("\nInvalid_Opcode_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Device_Not_Available_Exception() {
    //clear();
    printf("\nDevice_Not_Available_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Double_Fault_Exception() {
    //clear();
    printf("\nDouble_Fault_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Coprocessor_Segment_Overrun_Exception() {
    //clear();
    printf("\nCoprocessor_Segment_Overrun_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Invalid_TSS_Exception() {
    //clear();
    printf("\nInvalid_TSS_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Segment_Not_Present_Exception() {
    //clear();
    printf("\nSegment_Not_Present_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Stack_Segment_Fault_Exception() {
    //clear();
    printf("\nStack_Segment_Fault_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void General_Protection_Exception() {
    //clear();
    printf("\nGeneral_Protection_Exception\n");
    //disable_irq(1);
    //// while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Page_Fault_Exception() {
    //clear();
    printf("\nPage_Fault_Exception\n");
    //disable_irq(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Math_Fault_Exception() {
    //clear();
    printf("\nMath_Fault_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Alignment_Check_Exception() {
    //clear();
    printf("\nAlignment_Check_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void Machine_Check_Exception() {
    //clear();
    printf("\nMachine_Check_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}

/* exception handler */
void SIMD_Floating_Point_Exception() {
    //clear();
    printf("\nSIMD_Floating_Point_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}


/* exception handler */
void Intel_reserved_Exception() {
    //clear();
    printf("\nIntel_reserved_Exception\n");
    //disable_irq(1);
    // while(1);
    raise_exception();
    halt(0);
}
