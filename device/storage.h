#ifndef __STORAGE_H
#define __STORAGE_H

#include <stdint.h>
#include <stddef.h>

#define NUM_EXT     7
#define BLOCK_SZ    32
#define DATA_START  1024

typedef struct extent {
  uint16_t index;
  uint16_t len;
} extent_t;

typedef struct inode {
  uint16_t type; // 0 = normal, 1 = directory, 2 = special
  extent_t extents[ NUM_EXT ];
  uint16_t size;
} inode_t;

// typedef struct directory {
//   dir_entry_t entries[]
// } dir_t;
//
// typedef struct dir_entry {
//
// } dir_entry_t;

void read_file(int sfid, void *x, int rw_ptr, size_t n);
void write_file(int sfid, void *x, int rw_ptr, size_t n);

#endif
