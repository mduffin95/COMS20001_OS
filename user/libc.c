#include "libc.h"

void yield() {
  asm volatile( "svc #0     \n"  );
}

int write( int fd, void *x, size_t n ) {
  int r;

  asm volatile( "mov r0, %1 \n"
                "mov r1, %2 \n"
                "mov r2, %3 \n"
                "svc #1     \n"
                "mov %0, r0 \n"
              : "=r" (r)
              : "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int read( int fd, void *x, size_t n ) {
  int r;

  asm volatile( "mov r0, %1 \n"
                "mov r1, %2 \n"
                "mov r2, %3 \n"
                "svc #2     \n"
                "mov %0, r0 \n"
              : "=r" (r)
              : "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int open( const char *pathname ) {
  int r;

  asm volatile( "mov r0, %1 \n"
                "svc #6     \n"
                "mov %0, r0 \n"
              : "=r" (r)
              : "r" (pathname)
              : "r0" );

  return r;
}

int close( int fd ) {
  int r;

  asm volatile( "mov r0, %1 \n"
                "svc #10     \n"
                "mov %0, r0 \n"
              : "=r" (r)
              : "r" (fd)
              : "r0" );

  return r;
}

int creat( const char *pathname ) {
  int r;

  asm volatile( "mov r0, %1 \n"
                "svc #8     \n"
                "mov %0, r0 \n"
              : "=r" (r)
              : "r" (pathname)
              : "r0" );

  return r;
}

int unlink( const char *pathname ) {
  int r;

  asm volatile( "mov r0, %1 \n"
                "svc #9     \n"
                "mov %0, r0 \n"
              : "=r" (r)
              : "r" (pathname)
              : "r0" );

  return r;
}

int lseek( int fd, int offset, int whence ) {
  int r;

  asm volatile( "mov r0, %1 \n"
                "mov r1, %2 \n"
                "mov r2, %3 \n"
                "svc #7     \n"
                "mov %0, r0 \n"
              : "=r" (r)
              : "r" (fd), "r" (offset), "r" (whence)
              : "r0", "r1", "r2" );

  return r;
}

pid_t fork() {
  pid_t r;

  asm volatile( "svc #3     \n"
                "mov %[result], r0 \n"
              : [result] "=r" (r)
              :
              : "r0" );
  return r;
}

void exit() {
  asm volatile( "svc #4 \n" );
}

void exec(int prty, char *path, char **argv) {
  asm volatile( "mov r0, %[pr] \n"
                "mov r1, %[pa] \n"
                "mov r2, %[a] \n"
                "svc #5 \n"
              :
              : [pr] "r" (prty), [pa] "r" (path), [a] "r" (argv)
              : "r0", "r1", "r2" );
}

uint8_t *chan(uint8_t key) {
  return (uint8_t *) &ipc + (key << 1);
}

uint8_t chan_receive(uint8_t key) {
  volatile uint8_t *c = chan(key);
  while( !*c ); //Wait for channel to be ready
  *c = 0;
  uint8_t res = *(c+1);
  return res;
}

void chan_send(uint8_t key, uint8_t val) {
  volatile uint8_t *c = chan(key);
  while( *c ); //If channel is already ready (so hasn't been read since last time) then wait
  *(c+1) = val;
  *c = 1;
}

void chan_set_ready(uint8_t key) {
  volatile uint8_t *c = chan(key);
  *c = 1;
}

int itoa( int x, char *buf ) {
  int i = 0;
  int r;
  do {
    buf[i] = x % 10 + '0';
    x /= 10;
    i++;
  } while( x > 0 );
  r = i;
  i--;
  char tmp;
  for( int j = 0; j < i; j++ ) {
    tmp = buf[i];
    buf[i] = buf[j];
    buf[j] = tmp;
    i--;
  }
  return r;
}
