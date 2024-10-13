#ifndef terminal
#define terminal


#include "types.h"
#include "lib.h"

#include "keyboard.h"
#include "system_call.h"
#include "scheduling.h"
#define VGA_WIDTH   80
#define TERMINAL_NUM 3
#define FOUR_KB 4096
#define  SCREEN_VID_MEM 0xB8000
int jammer;
typedef struct {
    int active;
    int base_pid;
    int program_pid;
    int terminal_id;
    int key_count;
    int enter;
    int screen_x;
    int screen_y;
    int write_to_curr_screen;
    uint8_t keyboard_buffer[128];
    uint8_t* terminal_vid_mem;
    uint8_t* mem_buffer[4096];
} terminal_t;
// current terimnal number
int curr_terminal_visible;
// current terimnal number
int curr_terminal_executing;
// current terimnal number
int next_terminal_executing;
// struct for terminals
terminal_t multi_terminal[TERMINAL_NUM];
//functions for terminal, see more details in terminal.c
//get keyboard input from the keyboard
extern void getChar(char input);
//initilize the terminal
extern int open_terminal(const uint8_t* filename);
//close the terminal
extern int close_terminal(int32_t fd);
//inteprete the buffer for the terminal
extern int read_terminal(int32_t fd, void* buf, int32_t nbytes);
//output the content of the buffer to the terminal
extern int write_terminal(int32_t fd, const void* buf, int32_t nbytes);
//helper function for updating cursor position
extern void update_cursor(int x, int y);
// initialize terminals
void init_terminal();
//switch terminal
int32_t switch_terminal(uint8_t terminal_id);
#endif

