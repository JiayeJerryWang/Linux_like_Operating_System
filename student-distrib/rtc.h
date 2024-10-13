
#ifndef RTC
#define RTC

#include "lib.h"
#include "i8259.h"
#include "types.h"

// we get the port from https://wiki.osdev.org/RTC
#define RTC_IDX_PORT    0x70
#define RTC_REG_PORT    0x71
#define REG_A_IDX       0x8A
#define REG_B_IDX       0x8B
#define REG_C_IDX       0x8C


// RTC functions declared here, see more details in rtc.c
void inti_RTC();

void RTC_handler();

int32_t RTC_open(const uint8_t* filename);

int32_t RTC_close(int32_t fd);

int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes);

#endif
