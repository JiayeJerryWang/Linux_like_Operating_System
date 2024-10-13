#include "paging.h"

/* void initial_paging();
 * Inputs: void
 * Return Value: none
 * Function: Initialize paging */
void initial_paging(){
    uint32_t i;
    
    //init directory
    //1024 for looping through each of index; directory have length 1024
    for (i = 0; i < 1024; i++){
        if (i == 0) {
            page_directory_entry[i].P = 1;
            page_directory_entry[i].R_W = 1;
            page_directory_entry[i].U_S = 0; // video mem
            page_directory_entry[i].PWT = 0;
            page_directory_entry[i].PCD = 0;
            page_directory_entry[i].A = 0;
            page_directory_entry[i].O = 0;
            page_directory_entry[i].PS = 0;
            page_directory_entry[i].G = 0;
            page_directory_entry[i].AVAIL = 0;
            page_directory_entry[i].PTA = (uint32_t) page_table_entry >> 12;
        } else if (i == 1) {
            page_directory_entry[i].P = 1;
            page_directory_entry[i].R_W = 1;
            page_directory_entry[i].U_S = 0; // kernel
            page_directory_entry[i].PWT = 0;
            page_directory_entry[i].PCD = 0;
            page_directory_entry[i].A = 0;
            page_directory_entry[i].O = 0;
            page_directory_entry[i].PS = 1;
            page_directory_entry[i].G = 1;
            page_directory_entry[i].AVAIL = 0;
            page_directory_entry[i].PTA = 0x400; // kernel at 0x400000, right shift 12 bits to get the correct shift, so we get 0x400
        //if the diractory is 33, meaning we are at the start of the video memory
        } else if (i == 33) {
            page_directory_entry[i].P = 1;
            page_directory_entry[i].R_W = 1;
            page_directory_entry[i].U_S = 1; 
            page_directory_entry[i].PWT = 0;
            page_directory_entry[i].PCD = 0;
            page_directory_entry[i].A = 0;
            page_directory_entry[i].O = 0;
            page_directory_entry[i].PS = 0;
            page_directory_entry[i].G = 0;
            page_directory_entry[i].AVAIL = 0;
            page_directory_entry[i].PTA = (uint32_t) page_table_video >> 12; // video memory start from 132MB
        } else {
            page_directory_entry[i].P = 0;
            page_directory_entry[i].R_W = 1;
            page_directory_entry[i].U_S = 0; 
            page_directory_entry[i].PWT = 0;
            page_directory_entry[i].PCD = 0;
            page_directory_entry[i].A = 0;
            page_directory_entry[i].O = 0;
            page_directory_entry[i].PS = 0;
            page_directory_entry[i].G = 0;
            page_directory_entry[i].AVAIL = 0;
            page_directory_entry[i].PTA = 0;
        }
    }
    /* init table */
    //1024 for looping through each of index; table have length 1024
    for (i = 0; i < 1024; i++){
        //0xB8 is the start of video memory
        if (i == 0xB8) {
            page_table_entry[i].P = 1;
            page_table_entry[i].R_W = 1;
            page_table_entry[i].U_S = 1;
            page_table_entry[i].PWT = 0;
            page_table_entry[i].PCD = 0;
            page_table_entry[i].A = 0;
            page_table_entry[i].D = 0;
            page_table_entry[i].PAT = 0;
            page_table_entry[i].G = 0;
            page_table_entry[i].AVAIL = 0;
            page_table_entry[i].PBA = i;    
        } else if (i == 0xB9) {
            page_table_entry[i].P = 1;
            page_table_entry[i].R_W = 1;
            page_table_entry[i].U_S = 1;
            page_table_entry[i].PWT = 0;
            page_table_entry[i].PCD = 0;
            page_table_entry[i].A = 0;
            page_table_entry[i].D = 0;
            page_table_entry[i].PAT = 0;
            page_table_entry[i].G = 0;
            page_table_entry[i].AVAIL = 0;
            page_table_entry[i].PBA = i;    
        } else if (i == 0xBA) {
            page_table_entry[i].P = 1;
            page_table_entry[i].R_W = 1;
            page_table_entry[i].U_S = 1;
            page_table_entry[i].PWT = 0;
            page_table_entry[i].PCD = 0;
            page_table_entry[i].A = 0;
            page_table_entry[i].D = 0;
            page_table_entry[i].PAT = 0;
            page_table_entry[i].G = 0;
            page_table_entry[i].AVAIL = 0;
            page_table_entry[i].PBA = i;  
        } else if (i == 0xBB) {
            page_table_entry[i].P = 1;
            page_table_entry[i].R_W = 1;
            page_table_entry[i].U_S = 1;
            page_table_entry[i].PWT = 0;
            page_table_entry[i].PCD = 0;
            page_table_entry[i].A = 0;
            page_table_entry[i].D = 0;
            page_table_entry[i].PAT = 0;
            page_table_entry[i].G = 0;
            page_table_entry[i].AVAIL = 0;
            page_table_entry[i].PBA = i;       
        } else {
            page_table_entry[i].P = 0;
            page_table_entry[i].R_W = 1;
            page_table_entry[i].U_S = 0;
            page_table_entry[i].PWT = 0;
            page_table_entry[i].PCD = 0;
            page_table_entry[i].A = 0;
            page_table_entry[i].D = 0;
            page_table_entry[i].PAT = 0;
            page_table_entry[i].G = 0;
            page_table_entry[i].AVAIL = 0;
            page_table_entry[i].PBA = i;     
        }

        // Note that the video memory will require you to add
        // another page mapping for the program, in this case a 4 kB page. It is not ok to simply change the permissions of the
        // video page located < 4MB and pass that address.
        // page_table_video[i].P = 0;
        // page_table_video[i].R_W = 1;
        // page_table_video[i].U_S = 0;
        // page_table_video[i].PWT = 0;
        // page_table_video[i].PCD = 0;
        // page_table_video[i].A = 0;
        // page_table_video[i].D = 0;
        // page_table_video[i].PAT = 0;
        // page_table_video[i].G = 0;
        // page_table_video[i].AVAIL = 0;
        // page_table_video[i].PBA = i;
    }

    /* enable paging */
    //https://wiki.osdev.org/Paging
    asm(
        "movl $page_directory_entry, %eax;"
        "mov %eax, %cr3;"
        "mov %cr4, %eax;"
        "or $0x00000010, %eax;"
        "mov %eax, %cr4;"
        "mov %cr0, %eax;"
        "or $0x80000001, %eax;"
        "mov %eax, %cr0;"
    );
    return;
}
