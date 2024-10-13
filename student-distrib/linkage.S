.global	rtc_linkage, keyboard_linkage, PIT_linkage
rtc_linkage:
    pushal
    pushfl
    call RTC_handler
    popfl
    popal
    iret

keyboard_linkage:
    pushal
    pushfl
    call keyboard_handler
    popfl
    popal
    iret

PIT_linkage:
    pushal
    pushfl
    call PIT_handler
    popfl
    popal
    iret

