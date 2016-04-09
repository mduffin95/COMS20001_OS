#include "storage.h"
#include "disk.h"

// Need to incorporate rw pointer
void read_file(int sfid, void *x, int rw_ptr, size_t n) { //sfid = fd
  // int address = sfid * sizeof( inode_t ) 2 * BLOCK_SZ;
  // int block = address / BLOCK_SZ; //There are 2 blocks before inode table
  // int offset = address - block * BLOCK_SZ; //Location within block
  // SFID corresponds directly to inode number.
  inode_t tmp_inode;
  disk_rd( sfid, (uint8_t *) &tmp_inode, BLOCK_SZ );
  disk_rd( tmp_inode.extents[0].index, (uint8_t *) x, n);
}

void write_file(int sfid, void *x, int rw_ptr, size_t n) {
  inode_t tmp_inode;
  disk_rd( sfid, (uint8_t *) &tmp_inode, BLOCK_SZ );
  disk_wr( tmp_inode.extents[0].index, (uint8_t *) x, n);
}
