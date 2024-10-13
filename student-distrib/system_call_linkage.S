
.global system_call_handler, halt, execute, read, write, open, close, getargs, vidmap
system_call_handler:
    # save registers
    pushl   %esi
    pushl   %edi
    pushl   %ebp
    pushl   %edx
    pushl   %ecx
    pushl   %ebx
    pushfl
    
    # check whether eax is in range, if not, go to fail
    cmpl    $1, %eax
    jb      fail
    cmpl    $10, %eax
    ja      fail
    jmp     *systable(, %eax, 4)

# when fail, goes to fail
fail:
    movl    $-1, %eax
    jmp     quit

# system call jump table
systable:
    .long spare, Halt, Execute, Read, Write, Open, Close, Getargs, Vidmap

spare: # empty, will never be used

# call for system halt function:
# int32_t halt (uint8_t status);
Halt:
    pushl   %ebx
    call    halt
    popl    %ebx
    jmp     quit

# call for system execute function:
# int32_t execute (const uint8_t* command);
Execute:
    pushl   %ebx
    call    execute
    popl    %ebx
    jmp     quit

# call for system read function:
# int32_t read (int32_t fd, void* buf, int32_t nbytes);
Read:
    pushl   %edx
    pushl   %ecx
    pushl   %ebx
    call    read
    popl    %ebx
    popl    %ecx
    popl    %edx
    jmp     quit

# call for system write function:
# int32_t write (int32_t fd, const void* buf, int32_t nbytes);
Write:
    pushl   %edx
    pushl   %ecx
    pushl   %ebx
    call    write
    popl    %ebx
    popl    %ecx
    popl    %edx
    jmp     quit

# call for system open function:
# int32_t open (const uint8_t* filename);
Open:
    pushl   %ebx
    call    open
    popl    %ebx
    jmp     quit

# call for system open function:
# int32_t close (int32_t fd);
Close:
    pushl   %ebx
    call    close
    popl    %ebx
    jmp     quit

# call for system getarg function:
# int32_t getargs (uint8_t* buf, int32_t nbytes);
Getargs:
    pushl   %ecx
    pushl   %ebx
    call    getargs
    popl    %ebx
    popl    %ecx
    jmp     quit

# call for system vidmap function:
# int32_t vidmap (uint8_t** screen_start);
Vidmap:
    pushl   %ebx
    call    vidmap
    popl    %ebx
    jmp     quit


# quit, restore registers and call iret
quit:
    popfl
    popl    %ebx
    popl    %ecx
    popl    %edx
    popl    %ebp
    popl    %edi
    popl    %esi
    iret
