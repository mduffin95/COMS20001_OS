#include "disk.h"
#include "constants.h"
#include <string.h>

void addr_puth( PL011_t* d, uint32_t x ) {
  PL011_puth( d, ( x >>  0 ) & 0xFF );
  PL011_puth( d, ( x >>  8 ) & 0xFF );
  PL011_puth( d, ( x >> 16 ) & 0xFF );
  PL011_puth( d, ( x >> 24 ) & 0xFF );
}

void data_puth( PL011_t* d, const uint8_t* x, int n ) {
  for( int i = 0; i < n; i++ ) {
    PL011_puth( d, x[ i ] );
  }
}

void data_geth( PL011_t* d,       uint8_t* x, int n ) {
  for( int i = 0; i < n; i++ ) {
    x[ i ] = PL011_geth( d );
  }
}

uint32_t disk_get_block_num() {
  int n = 2 * sizeof( uint32_t ); uint8_t x[ n ];

  for( int i = 0; i < RETRY; i++ ) {
      PL011_puth( UART1, 0x00 );        // write command
      PL011_putc( UART1, '\n' );        // write EOL

    if( PL011_geth( UART1 ) == 0x00 ) { // read  command
      PL011_getc( UART1       );        // read  separator
       data_geth( UART1, x, n );        // read  data
      PL011_getc( UART1       );        // read  EOL

      return ( ( uint32_t )( x[ 0 ] ) <<  0 ) |
             ( ( uint32_t )( x[ 1 ] ) <<  8 ) |
             ( ( uint32_t )( x[ 2 ] ) << 16 ) |
             ( ( uint32_t )( x[ 3 ] ) << 24 ) ;
    }
    else {
      PL011_getc( UART1       );        // read  EOL
    }
  }

  return -1;
}

uint32_t disk_get_block_len() {
  int n = 2 * sizeof( uint32_t ); uint8_t x[ n ];

  for( int i = 0; i < RETRY; i++ ) {
      PL011_puth( UART1, 0x00 );        // write command
      PL011_putc( UART1, '\n' );        // write EOL

    if( PL011_geth( UART1 ) == 0x00 ) { // read  command
      PL011_getc( UART1       );        // read  separator
       data_geth( UART1, x, n );        // read  data
      PL011_getc( UART1       );        // read  EOL

      return ( ( uint32_t )( x[ 4 ] ) <<  0 ) |
             ( ( uint32_t )( x[ 5 ] ) <<  8 ) |
             ( ( uint32_t )( x[ 6 ] ) << 16 ) |
             ( ( uint32_t )( x[ 7 ] ) << 24 ) ;
    }
    else {
      PL011_getc( UART1       );        // read  EOL
    }
  }

  return -1;
}

void wr_help( uint32_t block, const uint8_t *x, int n ) {
  for( int i = 0; i < RETRY; i++ ) {
      PL011_puth( UART1, 0x01 );        // write command
      PL011_putc( UART1, ' '  );        // write separator
       addr_puth( UART1, block);        // write address
      PL011_putc( UART1, ' '  );        // write separator
       data_puth( UART1, x, n );        // write data
      PL011_putc( UART1, '\n' );        // write EOL

    if( PL011_geth( UART1 ) == 0x00 ) { // read  command
      PL011_getc( UART1       );        // read  EOL

      return;
    }
    else {
      PL011_getc( UART1       );        // read  EOL
    }
  }
  return;
}

/*
 * Write from x starting at byte a. Write a total of n bytes.
 */
void disk_wr( uint32_t a, const uint8_t* x, int n ) {
  uint32_t block = a >> 5; //Division by 32
  uint32_t offset = a & 0x1F;
  if( offset ) {
    uint8_t buffer[ BLOCK_SZ ];
    disk_rd( block*BLOCK_SZ, buffer, offset );
    int y = BLOCK_SZ - offset;
    memcpy( buffer + offset, x, y );
    n -= y;
    x += y;
    wr_help( block, buffer, BLOCK_SZ );
    block++;
  }
  if( n > 0 ) {
    wr_help( block, x, n );
  }
  return;
}

/*
 * Read from byte a into x. Read a total of n bytes.
 */
void disk_rd( uint32_t a,       uint8_t* x, int n ) {
  uint32_t block = a >> 5; //Division by 32
  uint32_t offset = a & 0x1F;
  uint8_t buffer[ BLOCK_SZ ];
  int y;
  while( n > 0 ) {
    for( int i = 0; i < RETRY; i++ ) {
        PL011_puth( UART1, 0x02 );        // write command
        PL011_putc( UART1, ' '  );        // write separator
         addr_puth( UART1, block);        // write address
        PL011_putc( UART1, '\n' );        // write EOL

      if( PL011_geth( UART1 ) == 0x00 ) { // read  command
        PL011_getc( UART1       );        // read  separator
         data_geth( UART1, buffer, BLOCK_SZ);        // read  data
        PL011_getc( UART1       );        // read  EOL

        break;
      }
      else {
        PL011_getc( UART1       );        // read  EOL
      }
    }
    if( BLOCK_SZ - offset < n ) {
      y = BLOCK_SZ - offset;
    }
    else {
      y = n;
    }
    memcpy( x, buffer + offset, y );
    n -= y;
    x += y;
    block++;
    offset = 0;
  }
  return;
}
