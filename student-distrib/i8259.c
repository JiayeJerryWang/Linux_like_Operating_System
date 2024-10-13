/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
//master and slave shall be initialized to all mask up
uint8_t master_mask = 0xFF;
uint8_t slave_mask = 0xFF;


/* codes referenced from linux/i8259.c and O  */

/* void i8259_init(void);
 * Inputs: void
 * Return Value: none
 * Function: Initialize both PICs slave and master */
void i8259_init(void) {
    //0xff stand for all mask
    outb(0xff, MASTER_8259_PORT_DATA);
    outb(0xff, SLAVE_8259_PORT_DATA);
    //sending four initialization flags
    //master
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER , MASTER_8259_PORT_DATA);
    outb(ICW3_MASTER, MASTER_8259_PORT_DATA);
    outb(ICW4, MASTER_8259_PORT_DATA);
    //slave
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE , SLAVE_8259_PORT_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_PORT_DATA);
    outb(ICW4, SLAVE_8259_PORT_DATA);
    //enable the slave interrupt
    enable_irq(2);
}

/* void enable_irq(uint32_t irq_num);
 * Inputs: void
 * Return Value: none
 * Function:  Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint32_t now_idx;
    //start pointer of the right most irq
    uint8_t sub_pointer = 1;
    //each pic worth 0-7, s+if larger we process to the slave port
    if(irq_num > 7){
        //we shall focus on which one we need to changed; each contain 8 irq, so we add the shift
        now_idx = irq_num -8;
        sub_pointer  = sub_pointer << now_idx;
        slave_mask &= ~(sub_pointer);
        outb(slave_mask, SLAVE_8259_PORT_DATA);
    } else {
        now_idx = irq_num;
        sub_pointer  = sub_pointer << now_idx;
        master_mask &= ~(sub_pointer);
        outb(master_mask, MASTER_8259_PORT_DATA);
    }
}

/* void disable_irq(uint32_t irq_num);
 * Inputs: void
 * Return Value: none
 * Function:  Disable (mask) the specified IRQ */
 void disable_irq(uint32_t irq_num) {
    uint32_t now_idx;
    //start pointer of the right most irq
    uint8_t sub_pointer = 1;
    //each pic worth 0-7, s+if larger we process to the slave port
    if(irq_num > 7){
        //we shall focus on which one we need to changed; each contain 8 irq, so we add the shift
        now_idx = irq_num -8;
        sub_pointer  = sub_pointer << now_idx;
        slave_mask |= (sub_pointer);
        outb(slave_mask, SLAVE_8259_PORT_DATA);
    } else {
        now_idx = irq_num;
        sub_pointer  = sub_pointer << now_idx;
        master_mask |= (sub_pointer);
        outb(master_mask, MASTER_8259_PORT_DATA);
    }
}

/* void send_eoi(uint32_t irq_num);
 * Inputs: void
 * Return Value: none
 * Function:  Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    int mask = irq_num;
    //each pic length 8;in case larger than 7, we shall send end of interrupt for both of master and slave then
    if(irq_num > 7){
        //we shall focus on which one we need to changed; each contain 8 irq, so we add the shif
        mask = irq_num - 8;
        outb(EOI | mask, SLAVE_8259_PORT);
        //slave port the 2 on master, so we apply 0x02 on it
        outb(EOI | 0x02 ,MASTER_8259_PORT);
    } else {
        outb(EOI | mask,MASTER_8259_PORT);
    }
}
