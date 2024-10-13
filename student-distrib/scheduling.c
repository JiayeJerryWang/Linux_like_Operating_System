#include "scheduling.h"


/* init_PIT
 * 
 * initialize PIT and some needed varialbes
 * Inputs: none
 * Outputs: none
 * Side Effects: set the PIT freq by count
 */
void init_PIT(){
    // the frequency we need to set
    int count = 20000;
    // initialize the curr_exe and next_exe value
    curr_terminal_executing = 0;
    next_terminal_executing = 0;
    // for the used bits in mode/command reg, see https://wiki.osdev.org/Pit for more
    // output to mode/command reg, set to channel 1 in low/high bits, set to mode3 in square mode
    // 0x36 is 00110110 in binary
    outb(0x36, 0x43);
    //set the low bytes
    outb(count&0xFF, 0x40);
    // set the high bytes
	outb((count&0xFF00)>>8, 0x40);
    // enable irq 0
    enable_irq(0);
}


/* PIT_handler
 * 
 * the function used to handle PIT interupt
 * Inputs: none
 * Outputs: none
 * Side Effects: send end of interupt to pic, save old pcb and update the new pcb
 */
void PIT_handler(){
    // send end of interupt
    send_eoi(0);
    cli();

    //update next terminal number
    int i;
    for(i = 0; i < 3; i++) {
        // we only have 3 multi_term, so we use 3
        next_terminal_executing = (next_terminal_executing + 1) % 3;
        if (multi_terminal[next_terminal_executing].active == 1) {
            break;
        }
    }
    
    // save old pcb
    pcb_t *old_pcb = get_pcb(multi_terminal[curr_terminal_executing].program_pid);
    asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        :"=r" (old_pcb->esp), "=r" (old_pcb->ebp)
        :
        :"memory"
    );
    
    //switching to the next terminal 
    curr_terminal_executing = next_terminal_executing;
    
    // update new pcb
    pcb_t *new_pcb = get_pcb(multi_terminal[curr_terminal_executing].program_pid);
    map_physical_memory(multi_terminal[curr_terminal_executing].program_pid);
    //get value for tss and esp 
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (Eight_MB - Eight_KB * (new_pcb->pid) - 0x4);

    //use new pcb to overwrite the pcb and esp
    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        :
        : "r"(new_pcb->esp), "r"(new_pcb->ebp)
        : "memory"
    );
    sti();
}
