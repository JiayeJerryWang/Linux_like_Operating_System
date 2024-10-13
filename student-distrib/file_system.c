#include "file_system.h"


/* void file_system_init();
 * Inputs: uint32_t* file_system_start_addr
 * Return Value: none
 * Function: initialize file system 
 */
void file_system_init (uint32_t* file_system_start_addr) {
    boot_block = (boot_block_t*) (file_system_start_addr);
    inode_block_start = (inode_t*) (boot_block + 1);
    data_block_start = (uint8_t*) (inode_block_start + (boot_block->inode_count));
}

/* int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
 * Inputs: const uint8_t* fname: file name
 *         dentry_t* dentry: the directory entry struct to copy to
 * Return Value: 0 on success, -1 on failure
 * Function: Scans through the directory entries in the “boot block” to find the file name and 
 * populates the file name, file type, inode number of the corresponding directory entry in the “boot block” with given name into dentry
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
    int i;
    for (i = 0; i < (boot_block->dir_count); i++) {
        /* zero value indicates that the characters compared in both strings form the same string */
        if (((strncmp((int8_t*) fname, (int8_t*) boot_block->direntries[i].filename, FILENAME_LEN)) == 0) && (strlen((int8_t*) fname) <= 32)) {
            /* When successful, fill in the dentry t block passed as second argument with the file name, 
             * file type, and inode number for the file */
            strcpy(dentry->filename, boot_block->direntries[i].filename);
            dentry->filetype = boot_block->direntries[i].filetype;
            dentry->inode_num = boot_block->direntries[i].inode_num;
            /* return 0 on success */
            //read_dentry_by_index (i, &dentry);
            return 0;
        }
    }
    /* return -1 on non-existent file */
    return -1;
}

/* int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
 * Inputs: uint32_t index: index number of the directory entry in the “boot block”
 *         dentry_t* dentry: the directory entry struct to copy to
 * Return Value: 0 on success, -1 on failure
 * Function: Populates the file name, file type, inode number of the corresponding directory entry in the “boot block” with given index number into dentry 
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    if (index < (boot_block->dir_count)) {
        /* When successful, fill in the dentry t block passed as second argument with the file name, 
         * file type, and inode number for the file, then return 0 */
        strncpy(dentry->filename, boot_block->direntries[index].filename, FILENAME_LEN);
        dentry->filetype = boot_block->direntries[index].filetype;
        dentry->inode_num = boot_block->direntries[index].inode_num;
        /* return 0 on success */
        return 0;
    }
    /* return -1 on invalid index */
    return -1;
}

/* int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
 * Inputs: uint32_t inode: inode number of the file to read the data from
 *         uint32_t offset: read start position in the file
 *         uint8_t* buf: buf to copy the data
 *         uint32_t length: read up to length bytes
 * Return Value: the number of bytes read and placed in the buffer, -1 on failure
 * Function: reading up to length bytes starting from position offset in the file with inode number inode and 
 * returning the number of bytes read and placed in the buffer 
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    int i;
    uint32_t read_start_data_block_num = (offset / BLOCK_SIZE);
    uint32_t read_start_data_block_index = (offset % BLOCK_SIZE);
    uint8_t* data_position;
    uint32_t bytes_read_and_placed = 0;
    inode_t* inode_to_read;
    data_position = (uint8_t*) (data_block_start);
    inode_to_read = (inode_t*) (inode_block_start + inode);
    /* check if the inode is out of bound */
    if ((inode >= boot_block->inode_count) || (inode < 0)) {
        /* return -1 on invalid inode number */
        return -1;
    }

    /* check if a bad data block number is found within the file bounds of the given inode*/
    if ( inode_to_read->data_block_num[read_start_data_block_num] >= boot_block->data_count) {
        /* return -1 on invalid data block number */
        return -1;
    }

    /* Read up to length bytes starting from position offset in the file with inode number inode */
    for(i = 0; i < length; i++) {
        /* check if need to move to the next block number */
        if (read_start_data_block_index == BLOCK_SIZE) {
            read_start_data_block_num++;
            read_start_data_block_index = 0;
        }

        /* check if a bad data block number is found within the file bounds of the given inode*/
        if (inode_to_read->data_block_num[read_start_data_block_num] >= boot_block->data_count) {
            /* return -1 on invalid data block number */
            return -1;
        }

        /* check if reach end of file */
        if ((i + offset) == (inode_to_read->length)) {
            //return bytes_read_and_placed;
            break;
        }

        /* update the position to read: data start + data block number * data block size + data index */
        data_position = (uint8_t*) ((data_block_start) + (inode_to_read->data_block_num[read_start_data_block_num]) * BLOCK_SIZE + read_start_data_block_index);

        /* store the corresponding byte in buf, 1 byte at a time */
        memcpy(buf, data_position, 1);

        /* bytes already read + 1 */
        bytes_read_and_placed++;

        /* index to read + 1 to read the next data */
        read_start_data_block_index++;

        /* buf + 1 to store the next data */
        buf++;
    }

    /* return the number of bytes read and placed in the buffer */
    return bytes_read_and_placed;
}

/* initialize any temporary structures, return 0 */
int32_t file_open (const uint8_t* fname) {
    if (read_dentry_by_name(fname, &dircetory_entry) == 0) {
        return 0;
    }
    //read_dentry_by_name(fname, &dircetory_entry);
    return -1;
}

/* undo what you did in the open function, return 0 */
int32_t file_close (uint32_t fd) {
    return 0;
}

/* file_write should do nothing, return -1 */
int32_t file_write (uint32_t fd, const void* buf, uint32_t nbytes) {
    return -1;
}

/* reads count bytes of data from file into buf */
int32_t file_read (uint32_t fd, void* buf, uint32_t nbytes) {

/*--------------------------check point 2 file read ----------------------------------*/
    //int i;
    //int j;
    //uint8_t test_buf[187];
	//uint32_t bytes_to_read = nbytes;
    //uint32_t bytes_read;
    //dentry_t dircetory_entry;
    // printf("data_block_num:%d", boot_block->data_count);
    // printf("\n");
    //read_dentry_by_name((const uint8_t*) "frame0.txt", &dircetory_entry);
    //read_dentry_by_name((const uint8_t*) "pingpong", &dircetory_entry);
    // printf("file_name_to_read:");
    // for (j = 0; j < FILENAME_LEN; j++) {
    //     printf("%c", dircetory_entry.filename[j]);      
    // }
    // printf("\n");
    // printf("inode_num:%d", dircetory_entry.inode_num);
    // printf("\n");
    // inode_t* inode_to_read;
    // inode_to_read = (inode_t*) (inode_block_start + dircetory_entry.inode_num);
    // data_t* data_position;
    // data_position = (data_t*) (data_block_start + inode_to_read->data_block_num[0]);
    // printf("file_length:%d", inode_to_read->length);
    // printf("\n");
    // printf("first_data_block:%d", inode_to_read->data_block_num[0]);
    // printf("\n");
    // printf("second_data_block:%d", inode_to_read->data_block_num[1]);
    // printf("\n");
    // for (i = 0; i < 10; i++) {
    //     putc(data_position->data[i]);      
    // }
    //putc(data_position->data[0]);
    //printf("%d", data_position->data[0]);
    //bytes_read = read_data(dircetory_entry.inode_num, 0, buf, bytes_to_read);
    //read_data(dircetory_entry.inode_num, 0, buf, bytes_to_read);
    // printf("bytes_read:%d", bytes_read);
    // printf("\n");

/*--------------------------check point 3 file read ----------------------------------*/
    //if buffer if NULL, return -1
    if (buf == NULL) {
        return -1;
    }
    //get the current index of the available pid
    int availble_pid = get_current_pid();
    //start with 8MB - 8KB, get the currently available pcb
    pcb_t *pcb = (pcb_t *)(Eight_MB - 0x2000 * (1 + availble_pid));
    //get the number of bytes of data 
    uint32_t bytes_read = read_data(pcb->fd[fd].inode, pcb->fd[fd].file_position, buf, nbytes);
    //if the bytes_read is valid
    if (bytes_read != -1) {
        pcb->fd[fd].file_position += bytes_read;
    }
    //get how many bytes of data being read
    return bytes_read;
}

/* opens a directory file, return 0 */
int32_t directory_open (const uint8_t* fname) {
    //return read_dentry_by_name (fname, dircetory_entry);
    return 0;
}

/* directory_close does nothing, return 0 */
int32_t directory_close (uint32_t fd) {
    return 0;
}

/* directory_write should do nothing, return -1 */
int32_t directory_write (uint32_t fd, const void* buf, uint32_t nbytes) {
    return -1;
}

/* directory_read read files filename by filename, including “.” */
int32_t directory_read (int32_t fd, void* buf, int32_t nbytes) {

/*--------------------------check point 2 directory read ----------------------------------*/
    // int i;
    // int j;
    // for (i = 0; i < boot_block->dir_count; i++) {
    //     dentry_t dircetory_entry_local = boot_block->direntries[i];
    //     // printf("file_name:");
    //     // for (j = 0; j < FILENAME_LEN; j++) {
    //     //     if (boot_block->direntries[i].filename[j] != NULL) {
    //     //         putc(boot_block->direntries[i].filename[j]);
    //     //         //printf("file_name: %c", dircetory_entry.filename[j]);
    //     //     }
    //     // }
    //     // printf("\n");
    //     // printf("file_type: %d", boot_block->direntries[i].filetype);
    //     // printf("\n");
    //     if (read_dentry_by_index(i, &dircetory_entry_local) == -1) {
    //         return -1; 
    //     } else {
    //         //strncpy((int8_t*) buf[i], (int8_t*) &(dircetory_entry.filename), FILENAME_LEN);
    //         printf("file_name:");
    //         for (j = 0; j < FILENAME_LEN; j++) {
    //             printf("%c", dircetory_entry_local.filename[j]);
    //             // if (dircetory_entry.filename[j] != NULL) {
    //             //     //putc(dircetory_entry.filename[j]);
    //             //     printf("%c", dircetory_entry.filename[j]);
    //             // }
    //         }
    //         // printf("\n");
    //         printf("   file_type: %d", dircetory_entry_local.filetype);
    //         inode_t* inode_to_read;
    //         inode_to_read = (inode_t*) (inode_block_start + dircetory_entry_local.inode_num);
    //         printf("   file_size: %d", inode_to_read->length);
    //         printf("\n");
    //     }
    // }

/*--------------------------check point 3 directory read ----------------------------------*/
    //if buffer if NULL, return -1
    if (buf == NULL) {
        return -1;
    }
    //get the current index of the available pid
    int availble_pid = get_current_pid();
    dentry_t dircetory_entry_local;
    // start with 8MB - 8KB, get the currently available pcb
    pcb_t *pcb = (pcb_t *)(Eight_MB - Eight_KB * (1 + availble_pid));
    //get the directory entry, if it is not valid, return 0
    if (read_dentry_by_index(pcb->fd[fd].file_position, &dircetory_entry_local) == -1) {
        return 0; 
    }
    //get the length of the file name
    uint32_t bytes_read = strlen((int8_t*) dircetory_entry_local.filename);
    //if the file name exceeds 32, we assign bytes_read to 32
    if (bytes_read >= FILENAME_LEN) {
        bytes_read = FILENAME_LEN;
    }
    //copy the data
    strncpy((int8_t*)buf, (int8_t*) dircetory_entry_local.filename, bytes_read);
    pcb->fd[fd].file_position++;
    //return the bytes read
    return bytes_read; 
}
