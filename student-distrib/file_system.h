#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "lib.h"
#include "types.h"
#include "system_call.h"

#define FILENAME_LEN 32
#define MAX_FILE_NUM 62
#define BLOCK_SIZE 4096

/* struct for directory entry */
typedef struct {
    int8_t filename[FILENAME_LEN];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserved[24];
} dentry_t;

/* struct for boot block */
typedef struct {
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[52];
    dentry_t direntries[63];
} boot_block_t;

/* struct for inode block */
typedef struct {
    int32_t length;
    int32_t data_block_num[1023];
} inode_t;


// typedef struct {
//     int8_t data[4096];
// } data_t;

//dentry_t* dircetory_entry;
boot_block_t* boot_block;
inode_t* inode_block_start;
uint8_t* data_block_start;
dentry_t dircetory_entry;

/* initialize file system */
extern void file_system_init (uint32_t* file_system_start_addr);

/* Scans through the directory entries in the “boot block” to find the file name and 
 * populates the file name, file type, inode number of the corresponding directory entry in the “boot block” with given name into dentry
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);

/* Populates the file name, file type, inode number of the corresponding directory entry in the “boot block” with given index number into dentry */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);

/* reading up to length bytes starting from position offset in the file with inode number inode and returning the number of bytes read and placed in the buffer */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/* initialize any temporary structures, return 0 */
int32_t file_open (const uint8_t* fname);

/* undo what you did in the open function, return 0 */
int32_t file_close (uint32_t fd);

/* file_write should do nothing, return -1 */
int32_t file_write (uint32_t fd, const void* buf, uint32_t nbytes);

/* reads count bytes of data from file into buf */
int32_t file_read (uint32_t fd, void* buf, uint32_t nbytes);

/* opens a directory file, return 0 */
int32_t directory_open (const uint8_t* fname);

/* directory_close does nothing, return 0 */
int32_t directory_close (uint32_t fd);

/* directory_write should do nothing, return -1 */
int32_t directory_write (uint32_t fd, const void* buf, uint32_t nbytes);

/* directory_read read files filename by filename, including “.” */
int32_t directory_read (int32_t fd, void* buf, int32_t nbytes);

#endif
