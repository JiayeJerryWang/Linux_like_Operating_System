#include "rtc.h"

volatile unsigned fg = 0;

/* inti_RTC
 * 
 * initialize RTC for use
 * Inputs: None
 * Outputs: none
 * Side Effects: enable PIE and set the frequency to 2hz, send irq request to pic
 * Files: rtc.c/h
 */
void inti_RTC() {
    uint8_t store;                      // a var used to store info from regs

    outb(REG_B_IDX, RTC_IDX_PORT);      // set the reg port to read reg B
    store = inb(RTC_REG_PORT);          // read the info from reg B
    store |=  0x40;                     // enable PIE
    outb(REG_B_IDX, RTC_IDX_PORT);      // set the reg port to read reg B
    outb(store, RTC_REG_PORT);          // set the reg B value

    cli();                              // clear interrupt flag before setting frequency
    outb(REG_A_IDX, RTC_IDX_PORT);      // set the reg port to read reg A
    store = inb(RTC_REG_PORT);          // read the info from reg A
    store &= 0xF0;                      // reset RS3 - RS0 to 0
    store |= 0x0F;                      // set frequency to 2hz, check for more frequency expressions from motolola RTC manual
    outb(REG_A_IDX, RTC_IDX_PORT);      // set the reg port to read reg A
    outb(store, RTC_REG_PORT);          // set the reg A value
    sti();                              // set interrupt flag after that

    // don't need to enable irq2 since enable irq8 will automatically enable irq2
    enable_irq(8);                      // enable irq8 - real time clock
}


/* RTC_handler
 * 
 * the interrupt handler for rtc used in the rtc linkage functions
 * Inputs: None
 * Outputs: none
 * Side Effects: clear interupt request in reg C, sent end of interrupt to pic
 * Files: rtc.c/h
 */
void RTC_handler() {
    //test_interrupts();                  // used for rtc testing
    //putc('1');

    outb(REG_C_IDX, RTC_IDX_PORT);      // set the reg port to read reg C
    inb(RTC_REG_PORT);                  // clear interupt request
    fg = 0;
    // don't need to enable irq2 since enable irq8 will automatically enable irq2
    send_eoi(8);                        // send eoi to irq8 - real time clock
}


/* RTC_open
 * 
 * the open function for the RTC driver
 * Inputs: const uint8_t* filename (not used)
 * Outputs: 0
 * Side Effects: initialize rtc
 * Files: rtc.c/h
 */
int32_t RTC_open(const uint8_t* filename) {
    inti_RTC();     // init rtc
    return 0;
}


/* RTC_open
 * 
 * the close function for the RTC driver
 * Inputs: int32_t fd (not used)
 * Outputs: 0
 * Side Effects: do nothing
 * Files: rtc.c/h
 */
int32_t RTC_close(int32_t fd) {
    return 0;
}


/* RTC_read
 * 
 * the read function for the RTC driver
 * Inputs: int32_t fd, void* buf, int32_t nbytes (all not used)
 * Outputs: 0
 * Side Effects: wait for a cycle of interrupt when read is called
 * Files: rtc.c/h
 */
int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes) {
    fg = 1;
    while (fg);     // wait for a cycle for read
    return 0;
}


/* RTC_read
 * 
 * the read function for the RTC driver
 * Inputs: int32_t fd       -- not used
 *         void* buf        -- the input frequency
 *         int32_t nbytes   -- the size of the buf in bytes
 * Outputs: 0 if success, -1 if fail
 * Side Effects: set frequency to the buf value, but only take in the 2's exponentials as frequency
 * Files: rtc.c/h
 */
int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes) {
    // check whether the nbytes is in range
    if (nbytes > 4 || nbytes < 1) {
        return -1;
    }

    // dereference the buf and check the frequency is in range (2, 1024)
    int16_t frequency = *((int16_t*)buf);
    if (frequency < 2 || frequency > 1024) {
        return -1;
    }
    unsigned store = frequency;
    unsigned counter = 0;       // the counter used to store whether the frequency is a 2^counter
    unsigned input = 16;        // the input value, if 2hz it is 1111 in binary, if 4hz it is 1110, check the motorola rtc file for more details
    uint8_t reg;                // used to store the data from port

    // check whether the input frequency is 2's exponential
    while (store >= 2) {
        if (store % 2 == 1) {
            return -1;
        }
        store = store / 2;
        counter++;
    }
    input -= counter;           // set the frequency ready for input

    // set the frequency to the rtc
    cli();
    outb(REG_A_IDX, RTC_IDX_PORT);
    reg = inb(RTC_REG_PORT);
    reg &= 0xF0;
    reg |= input;
    outb(REG_A_IDX, RTC_IDX_PORT);
    outb(reg, RTC_REG_PORT);
    sti();
    return 0;
}

