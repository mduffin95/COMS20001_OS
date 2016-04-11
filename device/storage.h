#ifndef __STORAGE_H
#define __STORAGE_H

#include <stdint.h>
#include <stddef.h>

#define NUM_EXT     7
#define BLOCK_SZ    32
#define INODE_START 2
#define DATA_START  258

typedef struct extent {
  uint16_t index;
  uint16_t len;
} extent_t;

typedef struct inode {
  uint16_t type; // 0 = normal, 1 = directory, 2 = special
  uint16_t size;
  extent_t extents[ NUM_EXT ];
} inode_t;

// typedef struct directory {
//   dir_entry_t entries[]
// } dir_t;
//
// typedef struct dir_entry {
//
// } dir_entry_t;

typedef struct open_file { //This needs to store the inode until the file is closed.
  int rw_ptr;
  int sfid;
  inode_t inode;
} of_t;

int read_file(int sfid, void *x, size_t n);
int write_file(int sfid, void *x, size_t n);
int open_file(int sfid);
int lseek_file(int fd, int offset, int whence);

int allocate(extent_t *e, int n);
int extend(extent_t *e, int n);
void format(void);

#endif
