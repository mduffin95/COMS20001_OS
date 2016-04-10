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
void write_file(int ufid, void *x, size_t n) { //n is number of blocks

  int sfid = of_table[ufid].sfid;
  inode_t tmp_inode;
  disk_rd( sfid, (uint8_t *) &tmp_inode, BLOCK_SZ );
  int offset = of_table[ufid].rw_ptr;
  int option = 1;
  int i = 0;
  while( tmp_inode.extents[i].index >= DATA_START && i < NUM_EXT ) { //Find out which extent the rw pointer lies within
    if( offset >= tmp_inode.extents[i].len ) {
      offset -= tmp_inode.extents[i].len;
    }
    else if( offset + n > tmp_inode.extents[i].len ) { //Case 3
      int diff = tmp_inode.extents[i].len - offset;
      disk_wr( tmp_inode.extents[i].index, (uint8_t *) x, diff );
      write_file( ufid, x + diff, n - diff);
      return;
    }
    else {
      option = 0;
      break;
    }
    i++;
  }
  if( option ) {
    //We ran out of extents and need to allocate another or extend an existing one.
    if( i > 0 ) { //There are some extents already allocated.
      int ext = extend( &(tmp_inode.extents[i-1]), offset + n );
      if( !ext ) {
        //Could not extend. Allocate a new extent.
        allocate( &(tmp_inode.extents[i]), offset + n );
      }
    }
    else { //Case 1. No extents. Allocate a new one.
      allocate( &(tmp_inode.extents[i]), offset + n );
    }
    write_file( ufid, x, n ); //Try again
    return;
  }
  else {
    //There is space within an existing extent. Also need to check whether
    //extent is large enough to contian the whole n blocks of the write.
    int block = tmp_inode.extents[i].index + offset;
    disk_wr( block, (uint8_t *) x, n * BLOCK_SZ);
    of_table[ufid].rw_ptr += n;
  }
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
  of_table[ufid].sfid = 0;
  of_table[ufid].rw_ptr = 0;
}

/*
 * Function that takes an extent and tries to extend it by n blocks.
 * Will round up to the nearest multiple of 8 blocks.
 */
int extend(extent_t *e, int n) {
  uint8_t bitmap[ BLOCK_SZ ] = {0};
  disk_rd( 1, bitmap, BLOCK_SZ );
  int i = (e->index + e->len) / 8; //Byte after
  int j = (n + 7) / 8; //Round up to nearest byte
  int k;
  for(k = 0; k < j; k++) {
    if( bitmap[i + k] ) { //If some are already allocated
      break;
    }
    bitmap[ i + k ] = 255;
  }
  e->len += k * 8;
  disk_wr( 1, bitmap, BLOCK_SZ );
  return k;
}

int allocate(extent_t *e, int n) {
  uint8_t bitmap[ BLOCK_SZ ] = {0};
  disk_rd( 1, bitmap, BLOCK_SZ );
  int i, j, k;
  k = (n + 7) / 8;
  while( i<BLOCK_SZ ) {
    for(j=i; j<i+k; j++) {
      if( bitmap[j] ){
        i = j + 1;
        break;
      }
    }
    for(j=i; j<i+k; j++) {
      bitmap[j] = 255;
    }
    e->index = i * 8;
    e->len = k * 8;
    disk_wr( 1, bitmap, BLOCK_SZ );
    return k * 8;
  }
  return 0;
}
