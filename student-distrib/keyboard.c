/* keyboard.c - initialize the keyboard device
 * vim:ts=4 noexpandtab */
 //define the input code here for non-print chracters based on osdev
#include "keyboard.h"  
// #define backspace       0x7F                                    //ascii code correspdonding to DEL
#define leftcontrol     0x1D
#define leftshift       0x2A
#define rightcontrol    0x1D
#define rightshift      0x2A
#define leftalt         0x38
#define capslock        0x3A
#define NumLock         0x45
#define ScrollLock      0x46
#define F1              0x3B
#define F2              0x3C
#define F3              0x3D
#define F4              0x3E
#define F5              0x3F
#define F6              0x40
#define F7              0x41
#define F8              0x42
#define F9              0x43
#define F10             0x44
#define F11             0x57
#define F12             0x58

//define a shift varibale to judge whether the shift button has been pressed
uint8_t bufferchar;
//variable to judge shift key
int shift = 0;
//varibale to judge caplosck key pressed or not
int cap = 0;
//variavle to judge capslock key released ot not 
int cap_ready = 1;
//vairbale to judge control key pressed or not 
int ctrl = 0;
//sum used to count how many chracters has been pressed
int sum = 0;

int alt = 0;
//initilial positions of the cursor x and y
unsigned cursor_x = 6;
unsigned cursor_y = 1;
//this is based on the order form the osdev.org
// we want a loopup table to print corresponding chracter on the terminal
char lookupcodes[89] = { 0, 'e', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
                        '\t', 'q', 'w', 'e', 'r', 't', 'y','u','i', 'o', 'p', '[', ']', '\n',
                        leftcontrol, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
                        leftshift, '\\', 'z','x','c','v', 'b', 'n', 'm', ',' , '.', '/', rightshift,
                        '*', leftalt, ' ', capslock, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NumLock, ScrollLock,
                        '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',        //for numpad
                        0, 0, 0, F11, F12};

//initilize the PIC IRQNUM 1 port for the keyboard
void init_keyboard(){
    enable_irq(1);
}


/* keyboard_handler
 * 
 * the handler functions that helps handle the differnet senerios when user presses the key and the cursor position on the screen
 * Inputs: no
 * Outputs:no
 * Side Effects: handle the user key input and send the input to the terminal buffer
*/
void keyboard_handler(){
    //https://www.win.tue.nl/~aeb/linux/kbd/scancodes-11.html
    //we want to take the input from port 0x60 and put to the terminal
    cli();
    uint8_t inputcode = inb(0x60);
    
    //the onput code is less than x80 to prevent the release overlap
    if (inputcode < 0x80) {
        //control has been presed
        if (inputcode == 0x1D) {
            ctrl = 1;
            send_eoi(1); 
            return;
        }

        if (inputcode == leftalt) {
            alt = 1;
            send_eoi(1);
            return;
        }


        //if ctrl + l is pressed, clear screen
        if (ctrl == 1 && inputcode == 0x26) {
            // clear screen has been presed and reset cursor position
            sum = 0;
            clear();
            open_terminal(0);
            cursor_x = 6;
            cursor_y = 0;
            send_eoi(1);
            return;
        }

        //if tab is pressed, output 4 space to the screen and update cursor and buffer
        if (inputcode == 0x0F) {
            puts("    ");
            getChar(lookupcodes[inputcode]);
            sum += 4;
            cursor_x += 4;
            update_cursor(cursor_x, cursor_y);
            send_eoi(1); 
            return;
        }         

        //judge whether the Capslock is pressed
        if (inputcode == 0x3A && cap_ready) {
            if (cap) {
                cap = 0;
            } else {
                cap = 1;
            }
            cap_ready = 0;
            send_eoi(1); 
            return;
        } 

        //if either left shift ot right shift is pressed, output the corresponding char on the screen
        if (inputcode == 0x2A || inputcode == 0x36) {
            shift = 1;
            send_eoi(1); 
            return;
        }

        //if alt + F1 
        if (alt == 1 && inputcode == F1) {
            switch_terminal(0);
            return;
        }

        //if alt + F2
        if (alt == 1 && inputcode == F2) {
            switch_terminal(1);
            return;
        }
        //if alt + F3     
        if (alt == 1 && inputcode == F3) {
            switch_terminal(2);
            return;
        }

        
        //handle the screen output for shift and caplock
        if (((inputcode >=0x2C) && (inputcode <= 0x32)) || ((inputcode >=0x1E) && (inputcode <= 0x26)) || ((inputcode >=0x10) && (inputcode <=0x19))) {
            //handle char only 
            sum += 1;
            
            //if the cursor is on the last line, set the x position to 0 if it exceed line limit
            if (cursor_y == 24 && cursor_x == 79) {
                cursor_x = 0;
            //same for other lines
            } else if(cursor_x == 79) {
                cursor_x = 0;
                cursor_y += 1;
            //in other case, just increases x
            } else {
                cursor_x += 1;
            }

            update_cursor(cursor_x, cursor_y);
            //if either Capslock or shift is pressed, get the capitalized version of the chracter
            if ((!cap && shift) || (cap && !shift)) {
                putc(lookupcodes[inputcode] -  32);
                getChar(lookupcodes[inputcode] -  32);
            } else {
                putc(lookupcodes[inputcode]);
                getChar(lookupcodes[inputcode]);
            }
        } else {
            //handle backspace case for cursor and keyboard, deduct the cursor position and sum by 1
            if(lookupcodes[inputcode] == '\b'){
                if(sum) {
                    sum -= 1;
                    cursor_x -= 1;
                    update_cursor(cursor_x, cursor_y);
                } else {
                    send_eoi(1);
                    return;
                }
            //handle enter case for cursor and keyboard
            } else if(lookupcodes[inputcode] == '\n') {
                sum = 0;
                //update cursor y based on cursor x position
                cursor_y += (cursor_x/80 + 1);
                //if y is already the last line, keep stays in the last line
                if (cursor_y > 24) {
                    cursor_y = 24;
                }
                cursor_x = 0;
                update_cursor(cursor_x, cursor_y);
            //else, just update the cursor muually
            } else {
                sum += 1;
                //if it is in the last line and the last chracter, reset x position
                if (cursor_y == 24 && cursor_x == 79) {
                    cursor_x = 0;
                //if x is 79, update line
                } else if(cursor_x == 79) {
                    cursor_x = 0;
                    cursor_y += 1;
                //else just updates x
                } else {
                    cursor_x += 1;
                }
                update_cursor(cursor_x, cursor_y);
            }

            //handle any other cases with need of mapping
            if (shift) {
                putc(shifthelper(lookupcodes[inputcode]));
                getChar(shifthelper(lookupcodes[inputcode]));
            } else {
                putc(lookupcodes[inputcode]);
                getChar(lookupcodes[inputcode]);
            }
        }  
    } else {
        //if the shift is released
        if (inputcode == 0xAA || inputcode  == 0xB6) {
            shift = 0;
        }
        //if the caps if realeased
        if (inputcode == 0xBA) {
            cap_ready = 1;
        }
        //if the control is realeased
        if ((inputcode == 0x9D)) {
            ctrl = 0;
        }
        if ((inputcode == 0xB8)) {
            alt = 0;
        }
    }
    send_eoi(1); 
}

/* shifthelper
 * 
 * the helper functions that helps handle the non-chracter cases with the shift button is pressed
 * Inputs: inputcode from the keyboard
 * Outputs: the alternative char correspbonding to the key
 * Side Effects: return alternative char
 */
char shifthelper(char inputcode){
    switch(inputcode) {
        case '`': return '~';
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case '\\': return '|';
        case ';': return ':';
        case '\'': return '"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        default: return inputcode;
    }
}


/* update_cursor_pos
 * 
 * the helper function helps use to update the cursor_x and cursor_y in keyboard.c
 * Inputs: x and y position
 * Outputs: None
 */
void update_cursor_pos(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

