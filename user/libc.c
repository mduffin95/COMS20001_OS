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
