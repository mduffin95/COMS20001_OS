#include "storage.h"
#include "disk.h"

of_t of_table[10];

// Need to incorporate rw pointer
void read_file(int ufid, void *x, size_t n) { //sfid = fd
  // int address = sfid * sizeof( inode_t ) 2 * BLOCK_SZ;
  // int block = address / BLOCK_SZ; //There are 2 blocks before inode table
  // int offset = address - block * BLOCK_SZ; //Location within block
  // SFID corresponds directly to inode number.
  int sfid = of_table[ufid].sfid;
  inode_t tmp_inode;
  disk_rd( sfid, (uint8_t *) &tmp_inode, BLOCK_SZ );
  disk_rd( tmp_inode.extents[0].index, (uint8_t *) x, n * BLOCK_SZ);
}


/*
 * Read inode, read bitmap, write bitmap, write inode, write block.
 *
 */
void write_file(int ufid, void *x, size_t n) {

  int sfid = of_table[ufid].sfid;
  inode_t tmp_inode;
  disk_rd( sfid, (uint8_t *) &tmp_inode, BLOCK_SZ );
  int offset = of_table[ufid].rw_ptr;
  int allocate = 1;
  int i = 0;
  int block;
  while( tmp_inode.extents[i].index >= DATA_START) {
    if((offset - tmp_inode.extents[i].len) > 0) {
      offset -= tmp_inode.extents[i].len;
    }
    else {
      allocate = 0;
      break;
    }
    i++;
  }
  if( allocate ) {
    //We ran out of extents and need to allocate another.

  }
  else {
    //There is space within an existing extent. Also need to check whether
    //extent is large enough to contian the whole n blocks of the write.
    block = tmp_inode.extents[i].index + offset;
  }


  disk_wr( block, (uint8_t *) x, n * BLOCK_SZ);
  of_table[ufid].rw_ptr += n;
}

int open_file(int sfid) {
  int ufid = 0;
  while( of_table[ufid].sfid ) {
    ufid++;
  }
  of_table[ufid].sfid = sfid;
  return ufid;
}

void close_file(int ufid) {
  of_table[ufid] = {0, 0};
}

/*
 * Function that takes an extent and tries to extend it.
 */
int extend(extent_t *e) {
  uint8_t bitmap[ BLOCK_SZ ] = {0};
  disk_rd( 1, bitmap, BLOCK_SZ );
}
