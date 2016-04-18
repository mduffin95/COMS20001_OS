#include "utils.h"
#include "libc.h"

void gcd_print( int x, int y, int r ) {
  char b1[16] = "gcd( ";
  char b2[16] = " ) = ";

  int s = itoa( x, b1+5 );
  *(b1+5+s) = ',';
  *(b1+6+s) = ' ';
  s += itoa( y, b1+7+s );
  write( -1, b1, 7+s );
  s = itoa( r, b2+5 );
  *(b2+5+s) = '\n';
  write( -1, b2, 6+s );
}

void prime_print( int x, int r ) {
  char b1[16] = "is_prime( ";
  char b2[16] = " ) = ";

  int s = itoa( x, b1+10 );
  write( -1, b1, 10+s );
  s = itoa( r, b2+5 );
  *(b2+5+s) = '\n';
  write( -1, b2, 6+s );
}

void weight_print( int x, int r ) {
  char b1[16] = "weight( ";
  char b2[16] = " ) = ";

  int s = itoa( x, b1+8 );
  write( -1, b1, 8+s );
  s = itoa( r, b2+5 );
  *(b2+5+s) = '\n';
  write( -1, b2, 6+s );
}
