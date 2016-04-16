#ifndef __STORAGE_H
#define __STORAGE_H

#include <stdint.h>
#include <stddef.h>
#include "constants.h"

//From http://stackoverflow.com/questions/523724/c-c-check-if-one-bit-is-set-in-i-e-int-variable
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
//I did the rest
#define SET_BIT(var,pos) (var | (1<<pos))
#define CLEAR_BIT(var,pos) (var & ~(1<<pos))

typedef struct extent {
  uint16_t index; //blocks
  uint16_t len; //blocks
} extent_t;

typedef struct inode {
  uint16_t type; // 0 = normal, 1 = directory, 2 = special
  uint16_t size; //bytes
  extent_t extents[ NUM_EXT ];
} inode_t;

typedef struct dir_entry {
  uint16_t sfid;
  char name[6];
} dir_entry_t;

typedef struct open_file { //This needs to store the inode until the file is closed.
  uint16_t rw_ptr; // <-- (Bytes) This could be stored in a per-process file descriptor table
  uint16_t sfid;
  inode_t inode;
} of_t;

int read_file(int fd, void *x, size_t n);
int write_file(int fd, void *x, size_t n);
int open_file(int fd);
int close_file(int fd);
int lseek_file(int fd, int offset, seek_t whence);
int unlink_file( int sfid );
int find_file( const char *pathname );
int creat_file( const char *pathname );

int allocate(extent_t *e, int n);
int extend(extent_t *e, int n);
void format(void);

#endif
