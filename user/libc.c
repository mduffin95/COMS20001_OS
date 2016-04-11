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
  return  (uint8_t *) (&ipc + (key << 1));
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
  while( *c );
  *(c+1) = val;
  *c = 1;
}
