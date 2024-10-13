#ifndef PAGING_H
#define PAGING_H
#include "types.h"
#include "x86_desc.h"
#include "lib.h"

/* initialize paging */
extern void initial_paging();

/* struct for page directory entry */
typedef struct __attribute__((packed)) PDE {
    uint16_t P          :1;
    uint16_t R_W        :1;
    uint16_t U_S        :1;
    uint16_t PWT        :1;
    uint16_t PCD        :1;
    uint16_t A          :1;
    uint16_t O          :1;
    uint16_t PS         :1;
    uint16_t G          :1;
    uint16_t AVAIL      :3;
    uint32_t PTA        :20;
}PDE_t;

/* struct for page tabel entry */
typedef struct __attribute__((packed)) PTE {
    uint16_t P          :1;
    uint16_t R_W        :1;
    uint16_t U_S        :1;
    uint16_t PWT        :1;
    uint16_t PCD        :1;
    uint16_t A          :1;
    uint16_t D          :1;
    uint16_t PAT        :1;
    uint16_t G          :1;
    uint16_t AVAIL      :3;
    uint32_t PBA        :20;
}PTE_t;

PDE_t page_directory_entry[1024] __attribute__((aligned(4096)));
PTE_t page_table_entry[1024] __attribute__((aligned(4096)));
PTE_t page_table_video[1024] __attribute__((aligned(4096)));
#endif
