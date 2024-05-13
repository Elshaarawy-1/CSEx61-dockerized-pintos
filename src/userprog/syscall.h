#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <list.h>
#include "threads/thread.h"
#include "threads/synch.h"
#include "threads/interrupt.h"
#include "threads/vaddr.h"
#include "process.h"
#include "devices/shutdown.h"
#include "devices/input.h"

void syscall_init (void);
void halt(void);
void exit(int status);
int wait(tid_t pid);

#endif /* userprog/syscall.h */
