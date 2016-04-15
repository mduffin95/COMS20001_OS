#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#define NUM_EXT     7
#define BLOCK_SZ    32
#define INODE_START 2
#define DATA_START  258

typedef enum {
  SEEK_SET,
  SEEK_CUR,
  SEEK_END
} seek_t;

#endif
