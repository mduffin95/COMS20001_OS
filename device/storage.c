#include "storage.h"
#include "disk.h"

of_t of_table[10];

/*
 * Read n bytes of the file given by fd into x.
 */
int read_file( int fd, void *x, size_t n ) {
  int sfid = of_table[fd].sfid;
  inode_t *inode = &(of_table[fd].inode);
  int offset = of_table[fd].rw_ptr;
  int total_read = 0;
  if( inode->extents[0].index < DATA_START ) return -1;
  int i = 0;
  while( inode->extents[i].index >= DATA_START && i < NUM_EXT ) {
    if( offset >= inode->extents[i].len ) {
      offset -= inode->extents[i].len;
    }
    else if( offset + n > inode->extents[i].len ) {
      int diff = inode->extents[i].len - offset;
      disk_rd( inode->extents[i].index + offset, (uint8_t *) x, diff * BLOCK_SZ );
      x += diff;
      n -= diff;
      total_read += diff;
      of_table[fd].rw_ptr += diff;
    }
    else {
      disk_rd( inode->extents[i].index + offset, (uint8_t *) x, n * BLOCK_SZ );
      total_read += n;
      of_table[fd].rw_ptr += n;
      break;
    }
    i++;
  }
  return total_read; //Change this.
}


/*
 * Read inode, read bitmap, write bitmap, write  inode, write block.
 *
 */
int write_file( int fd, void *x, size_t n ) { //n is number of blocks
  int sfid = of_table[fd].sfid;
  inode_t *inode = &(of_table[fd].inode);
  int offset = of_table[fd].rw_ptr;
  if( inode->extents[0].index < DATA_START ) goto cleanupD; //No extents.

  int i = 0;
  while( inode->extents[i].index >= DATA_START && i < NUM_EXT ) { //Find out which extent the rw pointer lies within
    if( offset >= inode->extents[i].len ) {
      offset -= inode->extents[i].len;
    }
    else if( offset + n > inode->extents[i].len ) { //Case 1
      goto cleanupB;
    }
    else goto cleanupA;
    i++;
  }
  goto cleanupC;

  cleanupA: //rw pointer is within extent. There is enough room.
    disk_wr( inode->extents[i].index + offset, (uint8_t *) x, n * BLOCK_SZ);
    of_table[fd].rw_ptr += n;
    return n;

  cleanupB: { //rw pointer is within extent but not enough room for whole write.
    int diff = inode->extents[i].len - offset;
    disk_wr( inode->extents[i].index + offset, (uint8_t *) x, diff * BLOCK_SZ );
    of_table[fd].rw_ptr += diff;
    write_file( fd, x + diff, n - diff);
    return n;
  }

  cleanupC: {//There are already extents but need another.
    int ext = extend( &(inode->extents[i-1]), offset + n );
    if( !ext ) //Could not extend. Allocate a new extent.
      allocate( &(inode->extents[i]), offset + n );
    else
      inode->size += ext; //Increase size of the file
    write_file( fd, x, n ); //Try again
    return n;
  }

  cleanupD: //There are no extents. Add one.
    allocate( &(inode->extents[i]), offset + n );
    write_file( fd, x, n ); //Try again
    return n;

}

/*
 * Navigate the directory structure to find the file provided.
 */
// int find_file( const char *pathname ) {
//
// }

int open_file( int sfid ) {
  int fd = 0;
  while( of_table[fd].sfid ) {
    fd++;
  }
  of_table[fd].sfid = sfid;
  disk_rd( sfid, (uint8_t *) &(of_table[fd].inode), BLOCK_SZ ); //Read inode into memory
  return fd;
}

void close_file(int fd) {
  disk_wr( of_table[fd].sfid, (uint8_t *) &(of_table[fd].inode), BLOCK_SZ ); //Write out inode
  of_table[fd].sfid = 0;
  of_table[fd].rw_ptr = 0;
}

/*
 * Returns the current offset in number of blocks from the beginning of the file.
 */
int lseek_file(int fd, int offset, int whence) {
  of_table[fd].rw_ptr += offset;
  return of_table[fd].rw_ptr;
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
