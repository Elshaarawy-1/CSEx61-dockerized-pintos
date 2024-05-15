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
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/malloc.h"

void syscall_init (void);
void halt(void);
void exit(int status);
int wait(tid_t pid);
bool create_file(const char* file_name, unsigned initial_size);
bool remove_file (const char* file_name);
int open_file(const char* file_name);
int file_size(int fd);
int read(int fd, void* buffer, unsigned size);
int write(int fd, void* buffer, unsigned size);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);
struct file_descriptor *get_file_by_fd(int fd);
bool is_valid_pointer(const void *ptr);

#endif /* userprog/syscall.h */
