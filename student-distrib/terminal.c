#include "terminal.h"

/* getChar
 * 
 * takes input from the keyboard and handles coresspondingly
 * Inputs: char
 * Outputs: none
 * Side Effects: takes keyboard input and handles them
 */
void getChar(char input) {
    if (multi_terminal[curr_terminal_visible].key_count < 128) {
        //if input is \n, enter is pressed
        if (input == '\n') {
            multi_terminal[curr_terminal_visible].enter = 1;
        }
        //if input is \b, reduce buffer size by 1 and keycount by 1
        if (input == '\b' && multi_terminal[curr_terminal_visible].key_count > 0) {
            multi_terminal[curr_terminal_visible].keyboard_buffer[multi_terminal[curr_terminal_visible].key_count - 1] = NULL;
            multi_terminal[curr_terminal_visible].key_count -= 1;
        } else {
            //else, just updates buffer normally
            multi_terminal[curr_terminal_visible].keyboard_buffer[multi_terminal[curr_terminal_visible].key_count] = input;
            multi_terminal[curr_terminal_visible].key_count += 1;
        }
    } else {
        //same for other cases, when user presses more keys, the backspace and enter is still the same logic
        if (input == '\n') {
            multi_terminal[curr_terminal_visible].enter = 1;
        }
        if (input == '\b' ) {
            multi_terminal[curr_terminal_visible].key_count -= 1;
        } else {
            multi_terminal[curr_terminal_visible].key_count += 1;
        }
    }
}


/* open_terminal
 * initlize the initlial state for the terminal
 * Inputs: filename
 * Outputs: int
 * initilizes a terminal with ECE391> on the side
 */
int32_t open_terminal(const uint8_t* filename) {
    //initilize index to 0
    int index = 0;
    //initilize the keycount to 0
    multi_terminal[curr_terminal_visible].key_count = 0;
    //initilize the whole terminal buf to null
    //there is no chracter input initilially
    for(index = 0; index < 128; index++) {
        multi_terminal[curr_terminal_visible].keyboard_buffer[index] = NULL;
    }
    multi_terminal[curr_terminal_visible].write_to_curr_screen = 1;
    //clear screen
    clear();
    //puts("391OS>");
    //update the original position for the cursor
    update_cursor(0, 1);
    return 0;
}


/* close_terminal
 * display the terminal after we close it
 * Inputs: filename
 * Outputs: int
 * clears the terminal buffer and clears the screen
 */
int32_t close_terminal(int32_t fd) {
    int index;
    //clears the buffer and screen
    for(index = 0; index < 128; index++) {
        multi_terminal[curr_terminal_visible].keyboard_buffer[index] = NULL;
    }
    clear();
    return 0;
}


/* read_terminal
 * read the any inputs and save it to the terminal buffer
 * Inputs: filename, *buf, nbytes
 * Outputs: int 
 * fills the terminal buffer with the terminal input
 */
int32_t read_terminal(int32_t fd, void* buf, int32_t nbytes) {
    // if (fd == 1) {
    //     return -1;
    // }
    int index;
    //reading_task is to track the number of bytes shoule be outputed
    int reading_task;
    char* buff = (char*)buf;
    //if the input is smaller than 127, no overflow happens
    if(nbytes < 127) {
        reading_task = nbytes;
    //buffer overflows happens
    } else {
        reading_task = 127;
    }

    while (1) {
        if (multi_terminal[curr_terminal_visible].enter) {
            //returned is the bytes being returned, i tracks the input size 
            unsigned i, returned;
            returned = 0;
            //shall walk through all previous except the end;
            for (i = 0; i < reading_task; i++) {
                buff[i] = multi_terminal[curr_terminal_visible].keyboard_buffer[i];
                returned += 1;
                if (buff[i] == '\n') {
                    jammer = 0;
                    while(1) {
                        if (curr_terminal_executing == curr_terminal_visible) {
                            cli();
                            break;
                        }
                    }
                    break;
                }
            }
            //handle insuffic input;
            //always add enter to the end
            if(i > 0){
                if(buff[i] != '\n') {
                    buff[i] = '\n';
                    returned += 1;
                }
            }
            
            //clear buffer and reset buffer once finished
            multi_terminal[curr_terminal_visible].enter = 0;
            //reset key count
            multi_terminal[curr_terminal_visible].key_count = 0;
            for(index = 0; index < 128; index++) {
                multi_terminal[curr_terminal_visible].keyboard_buffer[index] = NULL;
            }
            if (returned == 127) {
                returned += 1;
            }
            return returned;
        }
    }
}


// old write_terminal, used when multi_term is not implemented
// int32_t write_terminal(int32_t fd, const void* buf, int32_t nbytes) {
//     int i;
//     cli();
//     char * to_write = (char*) buf;
//     //the bytes that need to be returned
//     int success = 0;

//     //output the content of the buf to the screen
//     for (i = 0; i < nbytes; i++) {
//         char x = to_write[i];
//         putc(x);
//         success += 1;
//     }
//     sti();
//     return success;
// }


/* write_terminal
 * write the content of the buffer to the terminal screen
 * Inputs: filename, *buf, nbytes
 * Outputs: int 
 * output the content sin the buffer to the terminal screen
 */
int32_t write_terminal(int32_t fd, const void* buf, int32_t nbytes) {
    //printf("write");
    cli();
    int i;
    char * to_write = (char*) buf;
    //the bytes that need to be returned
    int success = 0;

    //variable used for synchronization
    if(jammer){
        jammer = 0;
        sti();
        return 0;
    }

    //variable used for synchronization
    if(hotjam ){
        hotjam = 0;
        sti();
        return 0;
    }
    //printf("cur%d, exe%d \n", curr_terminal_visible, curr_terminal_executing);
    //output the content of the buf to the screen
    if(curr_terminal_visible == curr_terminal_executing){
        // when visible == exe, use putc to put to screen
        for (i = 0; i < nbytes; i++) {
            char x = to_write[i];
            //k_putc("c");
            //printf("!");
            putc(x);
            //k_putc(x);
            success += 1;
        }
    } else {
        // otherwise use k_putc to put in vid mem
        for (i = 0; i < nbytes; i++) {
            char x = to_write[i];
            //k_putc("k");
            k_putc(x);
            success += 1;
        }
    }
    sti();
    return success;
}


/* update_cursor
 * update the cursor location on the screen
 * Inputs: x,y
 * Outputs: no
 * automatically update the cursor locations 
 */
void update_cursor(int x, int y) {
    //This is the algorithm from OSdev 
    //we use VGA port 0x3D4 and 0x3D5, the mask values are all from OSdev
	uint16_t pos = y * VGA_WIDTH + x;
	outb(0x0F, 0x3D4);
	outb((uint8_t) (pos & 0xFF), 0x3D5);
	outb(0x0E, 0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF), 0x3D5);
    update_cursor_pos(x, y);
}


/* init_terminal
 * initialize terminal
 * Inputs: none
 * Outputs: none
 * used for multi_terminal, initialize the value we need
 */
void init_terminal() {
    int i;
    int index;

    // clear screen
    clear();
    // currently running terminal is terminal 0
    curr_terminal_visible = 0;
    curr_terminal_executing = 0;

    // set base terminal shell pid to 0
    multi_terminal[0].base_pid = 0;
    multi_terminal[1].base_pid = -1;
    multi_terminal[2].base_pid = -1;

    multi_terminal[0].program_pid = 0;
    multi_terminal[1].program_pid = -1;
    multi_terminal[2].program_pid = -1;

    // set terminal 0 to active, terminal 1,2 to not active
    multi_terminal[0].active = 1;
    multi_terminal[1].active = 0;
    multi_terminal[2].active = 0;

    // set the relative vidmem address
    multi_terminal[0].terminal_vid_mem = (uint8_t*)0xB9000;
    multi_terminal[1].terminal_vid_mem = (uint8_t*)0xBA000;
    multi_terminal[2].terminal_vid_mem = (uint8_t*)0xBB000;

    // set up the page table
    page_table_entry[0xB9].PBA = 0xB9;
    page_table_entry[0xBA].PBA = 0xBA;
    page_table_entry[0xBB].PBA = 0xBB;

    // init write_to_curr_screen
    multi_terminal[0].write_to_curr_screen = 1;
    multi_terminal[1].write_to_curr_screen = 1;
    multi_terminal[2].write_to_curr_screen = 1;

    // initialize terminals
    for (i = 0; i < TERMINAL_NUM; i++) {
        // assign each terminal a terminal id
        multi_terminal[i].terminal_id = i;   

        // initialize the keycount to 0
        multi_terminal[i].key_count = 0; 

        // initialize screen x
        multi_terminal[i].screen_x = 0;

        // initialize screen y
        multi_terminal[i].screen_y = 0; 

        // initilize the whole terminal buf to null
        // there is no chracter input initilially
        for(index = 0; index < 128; index++) {
            multi_terminal[i].keyboard_buffer[index] = NULL;
        }
    }

    //update the original position for the cursor
    update_cursor(1, 1);
}


/* switch_terminal
 * switch to the terminal we will show
 * Inputs: uint8_t terminal_id  -- the terminal id we will show
 * Outputs: 0
 * used for multi_terminal, switch to the terminal we need
 */
int32_t switch_terminal(uint8_t terminal_id) {
    cli();
    // if the new terminal is the same, send end of interupt and do nothing
    if (terminal_id == curr_terminal_visible) {
        send_eoi(1); 
        return 0;
    }

    // mem copy from curr screen to vid mem
    memcpy( multi_terminal[curr_terminal_visible].terminal_vid_mem, (uint8_t*)0xB8000, FOUR_KB);

    // switch to new terminal
    // if the new terminal is already activated
    if (multi_terminal[terminal_id].active == 1) {
        //used for syncrhonization
        jammer = 1;
        //assign new terminal id
        curr_terminal_visible = terminal_id;
        memcpy((uint8_t*)0xB8000, multi_terminal[curr_terminal_visible].terminal_vid_mem, FOUR_KB);
        //restore screen
        update_cursor(multi_terminal[curr_terminal_visible].screen_x, multi_terminal[curr_terminal_visible].screen_y);
        // send end of interupt
        send_eoi(1); 
        return 0;
    }
    
    curr_terminal_visible = terminal_id;
    memcpy((uint8_t*)0xB8000, multi_terminal[curr_terminal_visible].terminal_vid_mem, FOUR_KB);


    // new terminal has not been activated, set active to 1
    multi_terminal[curr_terminal_visible].active = 1;
    multi_terminal[curr_terminal_visible].base_pid = get_current_pid() + 1;
    multi_terminal[curr_terminal_visible].program_pid = get_current_pid() + 1;

    //assign the value to synchronize
    curr_terminal_executing = curr_terminal_visible;
    update_cursor(multi_terminal[curr_terminal_visible].screen_x, multi_terminal[curr_terminal_visible].screen_y);
    send_eoi(1); 
    execute((const uint8_t*)"shell");
    return 0;
}
