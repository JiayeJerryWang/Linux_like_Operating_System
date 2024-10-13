#include "x86_desc.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "terminal.h"

//initilize the PIC IRQNUM 1 port for the keyboard
extern void init_keyboard();

//we want to take the input from port 0x60 and put to the terminal
//the onput code is less than x80 to prevent the release overlap
extern void keyboard_handler();

//this help functions helps handles the non-charcater case when Shift button is pressed on keyboard
extern char shifthelper(char inputcode);

// the helper function helps use to update the cursor_x and cursor_y in keyboard.c
extern void update_cursor_pos(int x, int y);
