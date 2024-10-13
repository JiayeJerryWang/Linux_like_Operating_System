#ifndef _SYSTEM_CALL_H
#define _SYSTEM_CALL_H

#include "lib.h"
#include "types.h"
#include "file_system.h"
#include "rtc.h"
#include "terminal.h"
#include "paging.h"
#include "system_call_linkage.h"

#define MAX_FD_NUM 8
#define MAX_PROCESS_NUM 6
#define MAX_CMD_LENGTH 32
#define MAX_ARG_LENGTH 128
#define EXE_CHECK_LENGTH 4
#define LOAD_ADDR 0x8048000
#define Eight_MB 0x800000
#define Eight_KB 0x2000
#define PROGRAM_IMG_START 0x8000000
#define PROGRAM_IMG_END 0x8400000

//used for synchronization
int hotjam;

/* struct for fd */
typedef struct {
    int32_t (*open) (const uint8_t* filename);
    int32_t (*close) (int32_t fd);
    int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
} fop_t;


/* struct for fd */
typedef struct {
    int32_t* file_operation;
    uint32_t inode;
    uint32_t file_position;
    uint32_t flag;
} fd_t;


/* struct for pcb */
//pid process id
//parent_id the id of the base shell
//parent esp: esp of the base shell
//parent ebp: ebp of the base shell
//ebp: ebp of the current running process
//esp: esp of the current running process
//active: whether the terminal is active or not
//arg: argument
typedef struct {
    fd_t fd[MAX_FD_NUM];
    uint32_t pid;
    uint32_t parent_id;
    uint32_t parent_esp;
    uint32_t parent_ebp;
    uint32_t esp;
    uint32_t ebp;
    uint32_t active;
    uint8_t arg[MAX_ARG_LENGTH];
    //uint32_t ret_addr;
} pcb_t;

/* ------------------- check point 3 -------------------- */

int32_t halt (uint8_t status);

int32_t execute (const uint8_t* command);

int32_t read (int32_t fd, void* buf, int32_t nbytes);

int32_t write (int32_t fd, const void* buf, int32_t nbytes);

int32_t open (const uint8_t* filename);

int32_t close (int32_t fd);

/* ------------------- check point 4 -------------------- */

int32_t getargs (uint8_t* buf, int32_t nbytes);

int32_t vidmap (uint8_t** screen_start);

int32_t set_handler (int32_t signum, void* handler_address);

int32_t sigreturn (void);


/* ------------------- helper functions -------------------- */

int32_t get_pid();

int32_t get_current_pid();

int32_t pop_pid();

pcb_t *get_pcb(uint8_t pid);

void map_physical_memory(uint8_t pid);

void flush_tlb();

extern void raise_exception();

#endif
