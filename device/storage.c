#include "storage.h"
#include "disk.h"

of_t of_table[10];

// Need to incorporate rw pointer
int read_file(int ufid, void *x, size_t n) {
  int sfid = of_table[ufid].sfid;
  inode_t tmp_inode;
  disk_rd( sfid, (uint8_t *) &tmp_inode, BLOCK_SZ );
  int offset = of_table[ufid].rw_ptr;
  int total_read = 0;
  if( tmp_inode.extents[0].index < DATA_START ) return -1;
  int i = 0;
  while( tmp_inode.extents[i].index >= DATA_START && i < NUM_EXT ) {
    if( offset >= tmp_inode.extents[i].len ) {
      offset -= tmp_inode.extents[i].len;
    }
    else if( offset + n > tmp_inode.extents[i].len ) {
      int diff = tmp_inode.extents[i].len - offset;
      disk_rd( tmp_inode.extents[0].index + offset, (uint8_t *) x, diff * BLOCK_SZ );
      x += diff;
      n -= diff;
      total_read += diff;
    }
    else {
      disk_rd( tmp_inode.extents[0].index + offset, (uint8_t *) x, n * BLOCK_SZ );
      total_read += n;
      of_table[ufid].rw_ptr += n;
      break;
    }
    i++;
  }
  return total_read; //Change this.
}


/*
 * Read inode, read bitmap, write bitmap, write inode, write block.
 *
 */
int write_file(int ufid, void *x, size_t n) { //n is number of blocks
  int sfid = of_table[ufid].sfid;
  inode_t tmp_inode;
  disk_rd( sfid, (uint8_t *) &tmp_inode, BLOCK_SZ );
  int offset = of_table[ufid].rw_ptr;
  if( tmp_inode.extents[0].index < DATA_START ) goto cleanupD; //No extents.

  int i = 0;
  while( tmp_inode.extents[i].index >= DATA_START && i < NUM_EXT ) { //Find out which extent the rw pointer lies within
    if( offset >= tmp_inode.extents[i].len ) {
      offset -= tmp_inode.extents[i].len;
    }
    else if( offset + n > tmp_inode.extents[i].len ) { //Case 1
      goto cleanupB;
    }
    else goto cleanupA;
    i++;
  }
  goto cleanupC;

  cleanupA: //rw pointer is within extent. There is enough room.
    disk_wr( tmp_inode.extents[i].index + offset, (uint8_t *) x, n * BLOCK_SZ);
    of_table[ufid].rw_ptr += n;
    return n;

  cleanupB: { //rw pointer is within extent but not enough room for whole write.
    int diff = tmp_inode.extents[i].len - offset;
    disk_wr( tmp_inode.extents[i].index + offset, (uint8_t *) x, diff * BLOCK_SZ );
    of_table[ufid].rw_ptr += diff;
    write_file( ufid, x + diff, n - diff);
    return n;
  }

  cleanupC: {//There are already extents but need another.
    int ext = extend( &(tmp_inode.extents[i-1]), offset + n );
    if( !ext ) //Could not extend. Allocate a new extent.
      allocate( &(tmp_inode.extents[i]), offset + n );
    else
      tmp_inode.size += ext; //Increase size of the file
    disk_wr( sfid, (uint8_t *) &tmp_inode, BLOCK_SZ ); //Write modified inode to disk.
    write_file( ufid, x, n ); //Try again
    return n;
  }

  cleanupD: //There are no extents. Add one.
    allocate( &(tmp_inode.extents[i]), offset + n );
    disk_wr( sfid, (uint8_t *) &tmp_inode, BLOCK_SZ ); //Write modified inode to disk.
    write_file( ufid, x, n ); //Try again
    return n;

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
 * Returns the current offset in number of blocks from the beginning of the file.
 */
int lseek(int ufid, int offset, int whence) {
  of_table[ufid].rw_ptr += offset;
  return of_table[ufid].rw_ptr;
}

/*
 * Function that takes an extent and tries to extend it by n blocks.
 * Will round up to the nearest multiple of 8 blocks.
 */
int extend(extent_t *e, int n) { //Round n up to the nearest multiple of 8
  uint8_t bitmap[ BLOCK_SZ ] = {0};
  disk_rd( 1, bitmap, BLOCK_SZ );
  int i = (e->index + e->len - DATA_START) >> 3;
  int j = (n + 7) >> 3;
  int k;
  for(k = 0; k < j; k++) {
    if( bitmap[i + k] ) { //If some are already allocated
      break;
    }
    bitmap[ i + k ] = 255;
  }
  e->len += k * 8;
  disk_wr( 1, bitmap, BLOCK_SZ );
  return k * 8;
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

void format(void) {
  uint8_t bitmap[ BLOCK_SZ ] = { 255 };
  disk_wr( 1, bitmap, BLOCK_SZ ); //8 data blocks allocated
  bitmap[0] = 1;
  disk_wr( 0, bitmap, BLOCK_SZ ); //One inode
  bitmap[0] = 0;
  bitmap[2] = 0x08;
  bitmap[4] = 0x02;
  bitmap[5] = 0x01;
  bitmap[6] = 0x08;
  disk_wr( 2, bitmap, BLOCK_SZ );
}
