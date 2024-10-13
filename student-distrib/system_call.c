#include "system_call.h"

/* flag for exception*/
int exception_flag = 0;
//int hotjam;
// maximum 6 programs can be ran
uint8_t process[MAX_PROCESS_NUM] = {0, 0, 0, 0, 0, 0};
// array for holding the file operation
int32_t *file_operation[4] = {(int32_t *)file_open, (int32_t *)file_close, (int32_t *)file_read, (int32_t *)file_write};
// array for holding the directory opertation
int32_t *directory_operation[4] = {(int32_t *)directory_open, (int32_t *)directory_close, (int32_t *)directory_read, (int32_t *)directory_write};
// array for the rtc operation
int32_t *rtc_operation[4] = {(int32_t *)RTC_open, (int32_t *)RTC_close, (int32_t *)RTC_read, (int32_t *)RTC_write};
// array for the terminal operation
int32_t *terminal_operation[4] = {(int32_t *)open_terminal, (int32_t *)close_terminal, (int32_t *)read_terminal, (int32_t *)write_terminal};
// array for the stdin
int32_t *stdin_operation[4] = {(int32_t *)open_terminal, (int32_t *)close_terminal, (int32_t *)read_terminal, (int32_t *)read_terminal};
// array for the stdout
int32_t *stdout_operation[4] = {(int32_t *)open_terminal, (int32_t *)close_terminal, (int32_t *)write_terminal, (int32_t *)write_terminal};

/* ---------------------------------------------------------------------------------------------------------------- */
/* -------------------------------- check point 3 system call handlers -------------------------------------------- */
/* ---------------------------------------------------------------------------------------------------------------- */

/* void halt();
 * Inputs: ststus
 * Return Value: int 
 * Function: called after a system call is finished, we restore partent data, context switch, restore partent PCB, close FDs, and return from halt
 */
int32_t halt(uint8_t status)
{
    //cli();
    int32_t fd;
    uint32_t local_status;
    
    if (exception_flag == 1) {
        exception_flag = 0;
        // return 256 if an exception is raised
        local_status = 256;
    } else {
        local_status = status;
    }
    // printf("status: %d", local_status);
    //printf("halt called\n");
    page_table_video[curr_terminal_executing].P = 0;
    /* ----------- Restore parent data ----------- */
    //get the current pid number
    int curr_pid = multi_terminal[curr_terminal_executing].program_pid;

    if (curr_pid == multi_terminal[curr_terminal_executing].base_pid) {
        // while(1) {
        //     if (curr_terminal_executing == curr_terminal_visible) {
        //         cli();
        //         break;
        //     }
        // }
        cli();
        process[curr_pid] = 0;
        multi_terminal[curr_terminal_executing].active = 0;
        //used for syncrhonization
        hotjam = 1;
        execute((const uint8_t*)"shell");
    }

    pcb_t *current_pcb = get_pcb(curr_pid);
    // get parent pcb
    pcb_t *parent_pcb = get_pcb(current_pcb->parent_id);
    //free the current pcb
    process[multi_terminal[curr_terminal_executing].program_pid] = 0;
    multi_terminal[curr_terminal_executing].program_pid = parent_pcb->pid;
    multi_terminal[curr_terminal_executing].write_to_curr_screen = 1;
    //deactive the current process
    current_pcb->active = 0;
    /* ----------- Restore parent paging ----------- */
    map_physical_memory(parent_pcb->pid);
    /* ----------- Close any relevant FDs ----------- */
    //set the correponding fields in the FD to 0, clost the FD
    int i = 0;
    //fd size
    for (i = 0; i < 8; i++)
    {
        current_pcb->fd[i].flag = 0;
        current_pcb->fd[i].file_operation = NULL;
        current_pcb->fd[i].file_position = 0;
        current_pcb->fd[i].inode = 0;
    }
    //close file during halt
    close(fd);
    /* ----------- Jump to execute return ----------- */
    // set ss0 and esp0
    tss.ss0 = KERNEL_DS;
    // need to push 4 more address becuase we have four arguments
    tss.esp0 = (Eight_MB - Eight_KB * (parent_pcb->pid) - 0x4);
    //tss.esp0 = parent_pcb->esp;
    sti();
    // halt return
    asm volatile(
        "movl %%edx, %%eax;"
        "movl %%ecx, %%esp;"
        "movl %%ebx, %%ebp;"
        "jmp halt_ret;"
        // "leave;"
        // "ret;"
        :
        : "b"(current_pcb->parent_ebp), "c"(current_pcb->parent_esp), "d"(local_status)
        : "eax");
    return 0;
}


/* void execute();
 * Inputs: command
 * Return Value: int 
 * Function: execute a system call
 */
int32_t execute(const uint8_t *command)
{
    
    cli();
    //used for synchronization
    if(jammer && ! hotjam){
        sti();
        return 0;
    }
    int i;
    uint8_t cmd_start = 0;
    uint8_t cmd_end = 0;
    uint8_t arg_start = 0;
    uint8_t arg_end = 0;
    uint8_t cmd_length = 0;
    uint8_t arg_length = 0;
    dentry_t dentry;
    pcb_t *PCB;
    uint8_t cmd[MAX_CMD_LENGTH] = {};
    uint8_t arg[MAX_ARG_LENGTH] = {};
    uint8_t check_exe[EXE_CHECK_LENGTH];

    // save the current ebp
    register uint32_t ebp asm("ebp");

    // save the current esp
    register uint32_t esp asm("esp");

    // invalid command
    if (command == NULL)
    {
        //printf("invalid command 1");
        sti();
        return -1;
    }

    // command too long
    if (strlen((int8_t *)command) > 128)
    {
        //printf("invalid command 2");
        sti();
        return -1;
    }

    /* ----------------------------------------- Parse args: ----------------------------------------- */
    // get the command from the keyboard
    // if the start of the command is space, increase the start position of the command
    int input_cmd_length = strlen((int8_t*) command);
    while (command[cmd_start] == ' ' || command[cmd_start] == '\t')
    {
        if (cmd_start == input_cmd_length) {
            // printf("break0\n");
            break;
        }
        if (command[cmd_start] != ' '  && command[cmd_start] != '\t') {
            // printf("break1\n");
            break;
        }
        cmd_start++;
    }
    cmd_end = cmd_start;
    // if the end of the command is not space, inrease the end position of the comoman
    while (command[cmd_end] != ' ' || command[cmd_end] != '\t')
    {
        if (cmd_end == input_cmd_length) {
            break;
        }
        if (command[cmd_end] == ' ' || command[cmd_end] == '\t')
            break;
        cmd_end++;
    }
    // get the command length
    cmd_length = cmd_end - cmd_start;
    for (i = 0; i < cmd_length; i++)
    {
        cmd[i] = command[cmd_start + i];
    }
    //get the argument
    arg_start = cmd_end;
    while (command[arg_start] == ' ' || command[arg_start] == '\t')
    {
        if (arg_start == input_cmd_length) {
            break;
        }
        arg_start++;
    }
    arg_end = arg_start;
    while (command[arg_end] != ' ' || command[arg_end] != '\t')
    { 
        if (arg_end == input_cmd_length) {
            break;
        }
        arg_end++;
    }  
    arg_length = arg_end - arg_start;
    for (i = 0; i < arg_length; i++)
    {
        arg[i] = command[arg_start + i];
    }
    if (multi_terminal[curr_terminal_executing].active == 0) {
        multi_terminal[curr_terminal_executing].active = 1;
    }  
    /* ----------------------------------------- Executable check: ----------------------------------------- */
    // Does the file exist?
    if (read_dentry_by_name((uint8_t *)cmd, &dentry) == -1)
    {
        //printf("invalid command 3\n");
        sti();
        return -1;
    }
    // Is the file an EXE? (Hint: look in MP3 documentation for some “magic numbers”)
    // 0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46.
    read_data(dentry.inode_num, 0, check_exe, 4);
    if ((check_exe[0] != 0x7F) || (check_exe[1] != 0x45) || (check_exe[2] != 0x4C) || (check_exe[3] != 0x46))
    {
        printf("file is not exe\n");
        sti();
        return -1;
    }
    // Is the file valid?
    // Remember to get prog_eip from valid files

    /* ----------------------------------------- Set up program paging: ----------------------------------------- */
    int remained_process = 6; 
    int terminal_num = multi_terminal[0].active + multi_terminal[1].active + multi_terminal[2].active;
    if(terminal_num < 3){
        for (i = 0; i < 6; i++){
            remained_process -= process[i];
        }
        if(3-terminal_num >= remained_process ){
            printf("too many processes,  ");
            sti();
            return -1;
        }   
    }

    int pid = get_pid();
    //printf("execute pid:%d\n", pid);
    // process filled
    if (pid == -1)
    {
        printf("too many processes,  ");
        sti();
        return -1;
    }
    // printf("pid:%d\n",pid);
    map_physical_memory(pid);
    //flush_tlb();

    /* ----------------------------------------- Load file into memory: ----------------------------------------- */
    inode_t *inode = (inode_t *)(inode_block_start + dentry.inode_num);
    uint32_t file_size = inode->length;
    read_data(dentry.inode_num, 0, (uint8_t *)LOAD_ADDR, file_size);

    /* ----------------------------------------- Create PCB: ----------------------------------------- */
    // Give pcb memory
    // Set active
    // Set file descriptor
    PCB = (pcb_t *)(Eight_MB - Eight_KB * (1 + pid));
    PCB->pid = pid;
    PCB->parent_ebp = ebp;
    PCB->parent_esp = esp;
    PCB->ebp = ebp;
    PCB->esp = esp;
    PCB->parent_id = multi_terminal[curr_terminal_executing].program_pid;
    PCB->active = (uint32_t) 1;
    strcpy((int8_t *) PCB->arg, (int8_t *) arg);     

    // stdin store in fd[0]
    PCB->fd[0].file_operation = (int32_t *) stdin_operation;
    PCB->fd[0].inode = 0;
    PCB->fd[0].file_position = 0;
    PCB->fd[0].flag = (uint32_t) 1;

    // stdout store in fd[1]
    PCB->fd[1].file_operation = (int32_t *) stdout_operation;
    PCB->fd[1].inode = 0;
    PCB->fd[1].file_position = 0;
    PCB->fd[1].flag = (uint32_t) 1;

    multi_terminal[curr_terminal_executing].program_pid = pid;
    /* ----------------------------------------- Context Switch: ----------------------------------------- */
    // Create it’s own context switch
    // Push IRET context to kernel stack
    // IRET

    // simply set the stack pointer to the bottom of the 4 MB page already holding the executable image
    // All user level programs will be loaded in the page starting at 128MB (virtual mem)
    // minus an additional esp size, which is 4 bytes
    // 128MB + 4MB = 132MB = 0x8400000
    uint32_t user_esp = (0x08400000 - sizeof(uint32_t));
    // virtual address of the first instruction that should be executed. This information is stored as a 4-byte unsigned integer in bytes 24-27 of the executable,
    uint8_t prog_eip_buffer[4];
    read_data(dentry.inode_num, 24, prog_eip_buffer, 4);
    uint32_t prog_eip = *((uint32_t *)prog_eip_buffer);

    // set ss0 and esp0
    tss.ss0 = KERNEL_DS;

    // need to push 4 more address becuase we have four arguments
    tss.esp0 = (Eight_MB - Eight_KB * pid - sizeof(uint32_t));
    // iret (ds -> esp -> flag -> cs -> eip)
    // https://wiki.osdev.org/Getting_to_Ring_3
    //printf("reach asm");
    sti();
    asm volatile(
        "movw %%bx, %%ds;"
        "pushl %%ebx;" // push user ds
        "pushl %%edx;" // push user esp
        "pushfl;"      // push e flag
        "pushl %%ecx;" // push cs
        "pushl %%eax;" // push eip
        "iret;"
        "halt_ret:;"
        //"sti;"
        "leave;"
        "ret;"
        :
        : "a"(prog_eip), "b"(USER_DS), "c"(USER_CS), "d"(user_esp)
        : "memory");
    return 0;
}


/* read
 * 
 * system call read function
 * Use file operations jump table to call the corresponding read function
 * Inputs: int32_t fd       -- file descriptor
 *         void *buf        -- buffer
 *         int32_t nbytes   -- the size of buffer in bytes
 * Outputs: -1 if read fail, else return the value from other read function
 * Files: system_call.c/h
 */
int32_t read(int32_t fd, void *buf, int32_t nbytes)
{
    //printf("read called\n");
    //  check for invalid fd or buf input
    if (fd < 0 || fd > 7 || fd == 1 || buf == NULL)
    {
        return -1;
    }

    // start with 8MB - 8KB, get the currently available pcb
    //pcb_t *pcb = (pcb_t *)(Eight_MB - Eight_KB * (1 + get_current_pid()));
    pcb_t *pcb = get_pcb(multi_terminal[curr_terminal_visible].program_pid);
    if (pcb->fd[fd].flag == 0)
    {
        return -1;
    }

    // get and run the coresponding close function
    int32_t (*f)(int32_t, const void*, int32_t) = (void*)pcb->fd[fd].file_operation[2];
    // printf("read finish\n");
    return f(fd, buf, nbytes);
}


/* write
 * 
 * system call write function
 * Use file operations jump table to call the corresponding write function
 * Inputs: int32_t fd       -- file descriptor
 *         void *buf        -- buffer
 *         int32_t nbytes   -- the size of buffer in bytes
 * Outputs: -1 if write fail, else return the value from other write function
 * Files: system_call.c/h
 */
int32_t write(int32_t fd, const void *buf, int32_t nbytes)
{
    cli();
    //printf("write called\n");
    //   check for invalid fd or buf input
    if (fd < 1 || fd > 7 || buf == NULL)
    {
        sti();
        return -1;
    }
    //int curr_pid = get_current_pid();
    //printf("current pid%d\n", curr_pid);
    // start with 8MB - 8KB, get the currently available pcb
    //pcb_t *pcb = (pcb_t *)(Eight_MB - Eight_KB * (1 + curr_pid));
    pcb_t *pcb = get_pcb(multi_terminal[curr_terminal_executing].program_pid);
    if (pcb->fd[fd].flag == 0)
    {
        sti();
        return -1;
    }

    // get and run the coresponding close function
    int32_t (*f)(int32_t, const void*, int32_t) = (void*)pcb->fd[fd].file_operation[3];
    //sti();
    return f(fd, buf, nbytes);
}


/* open
 * 
 * system call open function
 * Find the file in the file system and assign an unused file descriptor
 * File descriptors need to be set up according to the file type
 * Inputs: const uint8_t *filename      -- the filename we need to open
 * Outputs: -1 if open fail, else file descriptor number
 * Files: system_call.c/h
 */
int32_t open(const uint8_t *filename)
{
    cli();
    //printf("open called\n");
    // printf("file name:%s\n",filename);
    int openstatus = 0;
    //   check invalid input
    if (filename == NULL)
    {
        //sti();
        return -1;
    }
    dentry_t dentry;
    // get the dentry and check whether it fail
    if (read_dentry_by_name((uint8_t *) filename, &dentry))
    {
        //sti();
        return -1;
    }
    // get the current pid we are working on
    //int availble_pid = get_current_pid();

    // start with 8MB - 8KB, get the currently available pcb
    // pcb_t *pcb = (pcb_t *)(Eight_MB - Eight_KB * (1 + availble_pid));
    pcb_t *pcb = get_pcb(multi_terminal[curr_terminal_executing].program_pid);
    int fd = -1;
    int i;
    /* the size of the fd is 8
       we have fd from 0-7
       fd 0 is for stdin
       fd 1 is for stdout
       so we start from 2
    */
    for (i = 2; i < 8; i++)
    {
        if (pcb->fd[i].flag == 0)
        {
            fd = i;
            break;
        }
    }

    // check if fd is full or not assigned
    if (fd == -1)
    {
        //sti();
        return -1;
    }

    if (dentry.filetype == 0) {
        // if it is a rtc file
        pcb->fd[fd].file_operation = (int32_t*)rtc_operation;
        pcb->fd[fd].inode = 0;
        pcb->fd[fd].file_position = 0;
        pcb->fd[fd].flag = 1;
        openstatus = RTC_open(filename);
    } else if (dentry.filetype == 1) {
        // if it is a directory type file
        pcb->fd[fd].file_operation = (int32_t*)directory_operation;
        pcb->fd[fd].inode = 0;
        pcb->fd[fd].file_position = 0;
        pcb->fd[fd].flag = 1;
        openstatus = directory_open(filename);
    } else {
        // other situation, use common file operation
        pcb->fd[fd].file_operation = (int32_t*)file_operation;
        pcb->fd[fd].inode = dentry.inode_num;
        pcb->fd[fd].file_position = 0;
        pcb->fd[fd].flag = 1;
        openstatus = file_open(filename);
    }

    // printf("open finish\n");

    if (openstatus == -1) {
        //sti();
        return -1;
    }

    // return the final file descriptin number
    //sti();
    return fd;
}


/* close:
 *
 * system call close function
 * Close the file descriptor passed in (set it to be available)
 * Check for invalid descriptors
 * Inputs: int32_t fd       -- the file descriptor number we need to close
 * Outputs: -1 if close fail, 0 if success
 * Files: system_call.c/h
 */
int32_t close(int32_t fd)
{
    cli();
    //printf("close called\n");
    // check for invalid fd input
    if (fd < 2 || fd > 7)
    {
        sti();
        return -1;
    }

    int32_t returned;

    // get the current pid we are working on
    //int curr_pid = get_current_pid();

    // start with 8MB - 8KB, get the currently available pcb
    // pcb_t *pcb = (pcb_t*)(Eight_MB - Eight_KB * (1 + curr_pid));
    pcb_t *pcb = get_pcb(multi_terminal[curr_terminal_executing].program_pid);
    // check the current file situation
    if (pcb->fd[fd].flag == 0) {
        sti();
        return -1;
    }

    // run the coresponding close function
    int32_t (*f)(int32_t) = (void*)pcb->fd[fd].file_operation[1];
    returned = f(fd);
    // check whether the close called success
    if (returned) {
        sti();
        return -1;
    }
    pcb->fd[fd].file_operation = NULL;
    // clear the set values
    pcb->fd[fd].file_position = 0;
    pcb->fd[fd].flag = 0;
    pcb->fd[fd].inode = 0;
    sti();
    return 0;
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* -------------------------------- check point 4 system call handlers -------------------------------------------- */
/* ---------------------------------------------------------------------------------------------------------------- */

/* getargs:
 *
 * system call getargs function
 * Inputs: uint8_t* buf, int32_t nbytes
 * Outputs: 0 if success, -1 if fail
 */
int32_t getargs (uint8_t* buf, int32_t nbytes) {
    //cli();
    //printf("getarg called\n");
    if (buf == NULL) {
        //sti();
        return -1;
    }
    //int curr_pid = get_current_pid();
    // start with 8MB - 8KB, get the currently available pcb
    //pcb_t *current_pcb = (pcb_t *)(Eight_MB - Eight_KB * (1 + curr_pid));
    pcb_t *current_pcb = get_pcb(multi_terminal[curr_terminal_visible].program_pid);
    //printf("argument:%s\n", (uint8_t *) current_pcb->arg);

    // if argument is empty, return -1;
    if (strlen((int8_t *) current_pcb->arg) == 0) {
        // printf("getarg fail");
        //sti();
        return -1;
    }
    strcpy((int8_t *) buf, (int8_t *) current_pcb->arg);
    // printf("buf:%s\n", (uint8_t *) buf);
    //sti();
    return 0;
}


/* vidmap:
 *
 * system call vidmap function
 * Inputs: uint8_t** screen_start
 * Outputs: 0 if success, -1 if fail
 */
int32_t vidmap (uint8_t** screen_start) {
    //cli();
    // null check
    if(! screen_start){
        //sti();
        return -1;
    }
    // range check
    //check is pointer is out of range
    if (((uint32_t)screen_start < PROGRAM_IMG_START) || ((uint32_t)screen_start > PROGRAM_IMG_END)) {
        //sti();
        return -1;
    }
    * screen_start = (uint8_t*)(PROGRAM_IMG_END + 0x1000 * curr_terminal_executing);
    //* screen_start = (uint8_t*)(PROGRAM_IMG_END);
    // set the 4kb  page paging for vidmap
    page_table_video[curr_terminal_executing].P = 1;
    page_table_video[curr_terminal_executing].R_W = 1;
    page_table_video[curr_terminal_executing].U_S = 1;
    page_table_video[curr_terminal_visible].PBA = 0xB8;
    // if(curr_terminal_executing == 0){
    //     page_table_video[0].PBA = 0xB8;
    //     page_table_video[1].PBA = 0xBA;
    //     page_table_video[2].PBA = 0xBB;
    // } else if(curr_terminal_executing == 1){
    //     page_table_video[0].PBA = 0xB9;
    //     page_table_video[1].PBA = 0xB8;
    //     page_table_video[2].PBA = 0xBB;
    // } else {
    //     page_table_video[0].PBA = 0xB9;
    //     page_table_video[1].PBA = 0xBA;
    //     page_table_video[2].PBA = 0xB8;
    // }
    flush_tlb();
    //sti();
    return 0;
}

/* set_handler:
 *
 * system call set_handler function
 * Inputs: int32_t signum, void* handler_address
 * Outputs:
 */
int32_t set_handler (int32_t signum, void* handler_address) {
    return -1;
}

/* sigreturn:
 *
 * system call sigreturn function
 * Inputs: void
 * Outputs:
 */
int32_t sigreturn (void) {
    return -1;
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* ----------------------------- helper functions for system call handler ----------------------------------------- */
/* ---------------------------------------------------------------------------------------------------------------- */

/* pcb_t get_pid();
 * Inputs: none
 * Return Value: current new created pid
 * Function:  shall loop through the process and update the process
 */
int32_t get_pid()
{
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
    {
        // check if the process has been occupied
        if (process[i] == 0)
        {
            //if not, we can occupy and return
            process[i] = 1;
            //printf("pid:%d\n", i);
            return i;
        }
    }
    //else we are run out of availible slot; return as falsy
    return -1;
}

/* pcb_t get_current_pid();
 * Inputs: none
 * Return Value: current pid
 * Function:  shall check and get the current pid
 */
int32_t get_current_pid()
{
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
    {
        // check if the process has been occupied
        if (process[i] == 0)
        {
            // return i - 1 if the current process is shell;
            // else return 0; yet we shall not step to the shell
            if (i == 0)
            {
                return i;
            }
            else
            {
                return i - 1;
            }
        }
    }
    // pid = 5 if process is filled
    return 5;
}


/* pcb_t *get_pcb(uint8_t pid);
 * Inputs: pid
 * Return Value: availible pcb
 * Function:  Return the pcb block according to the current pid
 */
pcb_t *get_pcb(uint8_t pid)
{
    // PCB starts at 8MB – 8KB * (PCB_number)
    return (pcb_t *)(Eight_MB - Eight_KB * (1 + pid));
}


/* void map_physical_memory(uint8_t pid);
 * Inputs: pid
 * Return Value: none
 * Function:  mapping the virtual memory into the physical memory
 */
void map_physical_memory(uint8_t pid)
{
    // Physical memory starts at 8MB + (process number * 4MB) ; 4MB is 0x400000
    // Process number starts from 0
    uint32_t physical_addr = Eight_MB + (pid * 0x400000);
    uint32_t idx = LOAD_ADDR / (0x400000);
    page_directory_entry[idx].P = 1;
    page_directory_entry[idx].R_W = 1;
    page_directory_entry[idx].U_S = 1;
    page_directory_entry[idx].PWT = 0;
    page_directory_entry[idx].PCD = 0;
    page_directory_entry[idx].A = 0;
    page_directory_entry[idx].O = 0;
    page_directory_entry[idx].PS = 1;
    page_directory_entry[idx].G = 0;
    page_directory_entry[idx].AVAIL = 0;
    page_directory_entry[idx].PTA = (uint32_t) physical_addr >> 12;
    flush_tlb();
}


/* void flush_tlb();
 * Inputs: none
 * Return Value: none
 * Function:  flush the TLB by reset the PBDR
 */
void flush_tlb()
{
    asm volatile(
        "movl %cr3, %eax;"
        "movl %eax, %cr3;");
}


/* void void raise_exception()
 * Inputs: none
 * Return Value: none
 * Function: raise the exception flag
 */
void raise_exception() {
    exception_flag = 1;
}
