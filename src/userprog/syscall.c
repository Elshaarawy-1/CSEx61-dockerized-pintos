#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
  printf ("system call!\n");
  int Sys_call = *(int *)f->esp;
  switch(Sys_call){
    case SYS_HALT:
      halt();
      break;
    case SYS_EXIT:
    {
      int status;
      status = *(int *)(f->esp + 4);
      exit(status);
      break;
    }
    case SYS_WAIT:
    {
      tid_t pid; 
      pid = *(tid_t *)(f->esp + 4);
      f->eax = wait(pid);
      break;
    }
    case SYS_EXEC:
    {
      //dah Ay klam ya Ahmed ya mostafa e3ml 4o8lk
      const char *cmd_line;
      cmd_line = *(const char **)(f->esp + 4);
      f->eax = process_execute(cmd_line);
      break;
    }
      break;
    //do rest of the system calls
    default:
      break;
  }
  thread_exit ();
}

void halt(void){
  printf("halt()\n");
  shutdown_power_off();
}

void exit(int status){
  struct semaphore *sema = thread_current()->parent->sema;
  thread_current()->exit_status = status;
  sema_up(sema);
  
  printf("exit(%d)\n", status);
  thread_exit();
}

int wait(tid_t pid){
  return process_wait(pid);
}