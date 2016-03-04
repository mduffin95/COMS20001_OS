#ifndef __LIBC_H
#define __LIBC_H

#include <stddef.h>
#include <stdint.h>
#include <types.h>

// cooperatively yield control of processor, i.e., invoke the scheduler
void yield();
pid_t fork();
void exit();

// write n bytes from x to the file descriptor fd
int write( int fd, void* x, size_t n );

#endif
