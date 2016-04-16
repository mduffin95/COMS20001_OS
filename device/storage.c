#include "storage.h"
#include "disk.h"
#include <string.h>

of_t of_table[10];

/*
 * Read n bytes of the file given by fd into x.
 * Returns -1 if the file hasn't been initialised yet.
 */
int read_file( int fd, void *x, size_t n ) {
  inode_t *inode = &(of_table[fd].inode);
  int offset = of_table[fd].rw_ptr;
  int total_read = 0;
  if( inode->extents[0].index < DATA_START || offset >= inode->size ) return -1;
  int i = 0;
  int len_bytes, index_bytes;
  while( inode->extents[i].index >= DATA_START && i < NUM_EXT ) {
    len_bytes   = inode->extents[i].len * BLOCK_SZ;
    index_bytes = inode->extents[i].index * BLOCK_SZ;
    if( offset >= len_bytes ) {
      offset -= len_bytes;
    }
    else if( offset + n > len_bytes ) {
      int diff = len_bytes - offset;
      disk_rd( index_bytes + offset, (uint8_t *) x, diff );
      x += diff;
      n -= diff;
      total_read += diff;
      of_table[fd].rw_ptr += diff;
    }
    else {
      disk_rd( index_bytes + offset, (uint8_t *) x, n );
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
 * Write to the file given by the file descriptor. Write the data contained in x.
 * Write a total of n bytes.
 */
int write_file( int fd, void *x, size_t n ) {
  inode_t *inode = &(of_table[fd].inode);
  int offset = of_table[fd].rw_ptr;
  if( inode->extents[0].index < DATA_START ) goto cleanupD; //No extents.

  int i = 0;
  while( inode->extents[i].index >= DATA_START && i < NUM_EXT ) { //Find out which extent the rw pointer lies within
    if( offset >= inode->extents[i].len*BLOCK_SZ ) {
      offset -= inode->extents[i].len*BLOCK_SZ;
    }
    else if( offset + n > inode->extents[i].len*BLOCK_SZ ) { //Case 1
      goto cleanupB;
    }
    else goto cleanupA;
    i++;
  }
  goto cleanupC;

  cleanupA: //rw pointer is within extent. There is enough room.
    disk_wr( inode->extents[i].index*BLOCK_SZ + offset, (uint8_t *) x, n);
    of_table[fd].rw_ptr += n;
    if( of_table[fd].rw_ptr > inode->size ) {
      inode->size = of_table[fd].rw_ptr;
    }
    return n;

  cleanupB: { //rw pointer is within extent but not enough room for whole write.
    int diff = inode->extents[i].len*BLOCK_SZ - offset;
    disk_wr( inode->extents[i].index*BLOCK_SZ + offset, (uint8_t *) x, diff );
    of_table[fd].rw_ptr += diff;
    write_file( fd, x + diff, n - diff);
    return n;
  }

  cleanupC: {//There are already extents but need more room.
    int ext = extend( &(inode->extents[i-1]), offset + n );
    if( !ext ) //Could not extend. Allocate a new extent.
      allocate( &(inode->extents[i]), offset + n );
    write_file( fd, x, n ); //Try again
    return n;
  }

  cleanupD: //There are no extents. Add one.
    allocate( &(inode->extents[i]), offset + n );
    write_file( fd, x, n ); //Try again
    return n;

}

/*
 * Navigate the directory structure to find the file provided. Returns the sfid.
 */
int find_file( const char *pathname ) {
  int root = open_file( 2 );
  dir_entry_t x;
  int bytes_read;
  do {
    bytes_read = read_file( root, &x, sizeof(dir_entry_t) );
  } while( strcmp( x.name, pathname ) && bytes_read > 0 );
  close_file( root );
  if( bytes_read > 0 ) {
    return x.sfid;
  }
  return 0;
}

/*
 * Returns the sfid of the newly created file.
 */
int creat_file( const char *pathname ) {
  int ff = find_file( pathname );
  if( !ff ) {
    uint8_t bitmap[ BLOCK_SZ ] = {0};
    disk_rd( 0, bitmap, BLOCK_SZ ); //Read inode bitmap into memory
    int i = 0;
    for(; i<BLOCK_SZ && bitmap[ i ] == 0xFF; i++);
    int j = 0;
    for(; j<8 && CHECK_BIT( bitmap[ i ], j ); j++);
    bitmap[ i ] = SET_BIT(bitmap[ i ], j);
    int sfid = i*8 + j + INODE_START;
    disk_wr( 0, bitmap, BLOCK_SZ );

    int root = open_file( 2 ); //Open directory file
    lseek_file( root, 0, SEEK_END ); //Put pointer at end of file.
    dir_entry_t x;
    x.sfid = sfid; //Copy across sfid
    strcpy( x.name, pathname ); //Copy across name
    write_file( root, &x, sizeof(dir_entry_t) ); //Write entry to directory file
    close_file( root );
    //No need to write inode to disk because it has all zero values.
    return sfid;
  }
  return 0;
}

/*
 * Open the file given by the system file identifier. Return a file descriptor.
 */
int open_file( int sfid ) {
  int fd = 0;
  while( of_table[fd].sfid ) {
    fd++;
  }
  of_table[fd].sfid = sfid;
  disk_rd( sfid*BLOCK_SZ, (uint8_t *) &(of_table[fd].inode), BLOCK_SZ ); //Read inode into memory
  return fd;
}

void close_file(int fd) {
  disk_wr( of_table[fd].sfid*BLOCK_SZ, (uint8_t *) &(of_table[fd].inode), BLOCK_SZ ); //Write out inode
  of_table[fd].sfid = 0;
  of_table[fd].rw_ptr = 0;
}

/*
 * Returns the current offset in number of blocks from the beginning of the file.
 */
int lseek_file(int fd, int offset, seek_t whence) {
  switch( whence ) {
    case SEEK_SET:
    of_table[ fd ].rw_ptr = offset;
    break;
    case SEEK_CUR:
    of_table[fd].rw_ptr += offset;
    break;
    case SEEK_END:
    of_table[ fd ].rw_ptr = of_table[ fd ].inode.size + offset;
    break;
  }
  return of_table[fd].rw_ptr;
}

/*
 * Function that takes an extent and tries to extend it by n bytes.
 * Will round up to the nearest multiple of 8 blocks.
 */
int extend(extent_t *e, int n) {
  uint8_t bitmap[ BLOCK_SZ ] = {0};
  disk_rd( 1*BLOCK_SZ, bitmap, BLOCK_SZ );
  int i = (e->index + e->len - DATA_START) >> 3; //Because bitmap is stored in bytes
  int j = (n + 31) >> 5; //Round up to nearest number of blocks.
  j = (j + 7) >> 3; //Round up to nearest multiple of 8 blocks.
  int k; //k*8 is number of blocks extended by.
  for(k = 0; k < j; k++) {
    if( bitmap[i + k] ) { //If some are already allocated
      break;
    }
    bitmap[ i + k ] = 255;
  }
  e->len += k * 8;
  disk_wr( 1*BLOCK_SZ, bitmap, BLOCK_SZ );
  return k << 8; //Convert to number of bytes
}

/*
 * Create a new extent that is n bytes large, rounded up to the nearest multiple
 * of 8 blocks.
 */
int allocate(extent_t *e, int n) {
  uint8_t bitmap[ BLOCK_SZ ] = {0};
  disk_rd( 1*BLOCK_SZ, bitmap, BLOCK_SZ );
  int i = 0, j = 0, k;
  k = (n + 31) >> 5;
  k = (k + 7) >> 3; // <-- number of sets of 8 blocks.
  while( i<BLOCK_SZ ) {
    for(j=i; j<i+k; j++) {
      if( bitmap[j] ){
        i = j + 1;
        break;
      }
    }
    for(j=i; j<i+k; j++) {
      bitmap[j] = 0xFF;
    }
    e->index = i * 8 + DATA_START;
    e->len = k * 8;
    disk_wr( 1*BLOCK_SZ, bitmap, BLOCK_SZ );
    return k << 8; //Convert to number of bytes
  }
  return 0;
}

void format(void) {
  uint8_t bitmap[ BLOCK_SZ ] = { 255 };
  disk_wr( 1*BLOCK_SZ, bitmap, BLOCK_SZ ); //8 data blocks allocated
  bitmap[0] = 1;
  disk_wr( 0, bitmap, BLOCK_SZ ); //One inode
  inode_t i;
  i.type = 1;
  i.size = 0;
  i.extents[ 0 ].index = 258;
  i.extents[ 0 ].len = 8;
  disk_wr( 2*BLOCK_SZ, (uint8_t *) &i, BLOCK_SZ );
}
