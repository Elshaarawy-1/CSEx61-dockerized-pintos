#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>

static void syscall_handler (struct intr_frame *);

/* lock for file operations */
struct lock file_lock;

/* current file descriptor number in range (2 -> 128) */
int current_fd = 2;

/* Contains info about the current file. */
struct file_descriptor
{
  struct file *file;      /* The File. */
  int fd;                 /* File descriptor number. */
  struct list_elem elem; /* List Element. */
};

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&file_lock);
}

static void
syscall_handler (struct intr_frame *f)
{
  int Sys_call = *(int *)f->esp;
  switch(Sys_call){
    case SYS_HALT:
      halt();
      break;
    case SYS_EXIT:
    {
      int status;
      status = *((int *)f->esp + 1);
      exit(status);
      break;
    }
    case SYS_WAIT:
    {
      tid_t pid; 
      pid = *((int *)f->esp + 1);
      f->eax = wait(pid);
      break;
    }
    case SYS_EXEC:
    {
      // printf("Esmail Execution\n");
      if (!is_valid_pointer((const void *)*((int *)f->esp + 1))) 
        exit(-1);
      char *cmd_line;
      cmd_line = (char *)*((int *)f->esp + 1);
      f->eax = process_execute(cmd_line);
      break;
    }
    case SYS_CREATE:
    {
      const char *file_name = (const char *)*((int *)f->esp + 1);
      int initial_size = (unsigned)*((int *)f->esp + 2);
      f->eax = create_file (file_name, initial_size);
      break;
    }
    case SYS_REMOVE: 
    {
      const char *file_name = (const char *)*((int *)f->esp + 1);
      f->eax = remove_file (file_name);
      break;
    }
    case SYS_OPEN: 
    {
      const char *file_name = (const char *)*((int *)f->esp + 1);
      f->eax = open_file (file_name);
      break;
    }
    case SYS_FILESIZE: 
    {
      int fd = (int)*((int *)f->esp + 1);
      f->eax = file_size(fd);
      break;
    }
    case SYS_READ: 
    {
      int fd = (int)*((int *)f->esp + 1);
      void *buffer = (void *)*((int *)f->esp + 2);
      unsigned size = (unsigned)*((int *)f->esp + 3);
      f->eax = read (fd, buffer, size);
      break;
    }
    case SYS_WRITE: 
    {
      int fd = (int)*((int *)f->esp + 1);
      void *buffer = (void *)*((int *)f->esp + 2);
      unsigned size = (unsigned)*((int *)f->esp + 3);
      f->eax = write (fd, buffer, size);
      break;
    }
    case SYS_SEEK: 
    {
      int fd = (int)*((int *)f->esp + 1);
      unsigned pos = (unsigned)*((int *)f->esp + 2);
      seek (fd, pos);
      break;
    }
    case SYS_TELL: 
    {
      int fd = (int)*((int *)f->esp + 1);
      f->eax = tell (fd);
      break;
    }
    case SYS_CLOSE: 
    {
      int fd = (int)*((int *)f->esp + 1);
      close (fd);
      break;
    }
    default:
      break;
  }
  return;
}

void halt(void){
  printf("halt()\n");
  shutdown_power_off();
}

//calls thread_exit which in turn calls process_exit
void exit(int status){
  struct thread *cur = thread_current();
  cur->exit_status = status;
  printf("%s: exit(%d)\n", cur->name, status);
  thread_exit();
}

int wait(tid_t pid){
  return process_wait(pid);
}

bool
create_file(const char* file_name, unsigned initial_size) 
{
  if (!is_valid_pointer(file_name)) 
      exit(-1);

  bool success;
  lock_acquire(&file_lock);
  success = filesys_create(file_name, initial_size);
  lock_release(&file_lock);
  return success;
}

bool 
remove_file (const char* file_name)
{
  if (!is_valid_pointer(file_name)) 
      exit(-1);
    
  bool success;
  lock_acquire(&file_lock);
  success = filesys_remove(file_name);
  lock_release(&file_lock);
  return success;
}

int
open_file(const char* file_name)
{
  if (!is_valid_pointer(file_name)) 
      exit(-1);

  struct file_descriptor *file_descriptor;
  int fd = -1;

  lock_acquire(&file_lock);
  struct file *file = filesys_open(file_name);

  if (file != NULL)
    {
      file_descriptor = calloc (1, sizeof(struct file_descriptor));
      file_descriptor->fd = current_fd;
      file_descriptor->file = file;
      list_push_back(&thread_current() -> files, &file_descriptor->elem);
      current_fd++;
      fd = file_descriptor->fd;
    }

  lock_release(&file_lock);
  return fd;
}

int
file_size(int fd)
{
  struct file_descriptor *file = get_file_by_fd(fd);
  int size = -1;
  if (file != NULL) 
    {
      lock_acquire(&file_lock);
      size = file_length(file->file);
      lock_release(&file_lock);
    }
  return size;
}

int
read(int fd, void* buffer, unsigned size)
{
  if (!is_valid_pointer(buffer))
      exit (-1);

  int bytes_read = 0;
  lock_acquire(&file_lock);
  if (fd == 0)   // stdin
    {
      unsigned counter = size;
      uint8_t *buf = (uint8_t *)buffer;   /* just casting  */ 
      while (counter--)
        {
           uint8_t c = input_getc ();  /* read a key from buffer */
           if (c == '\0') break;
           *buf = c;    /* store the read char into the buffer */
           buf++;      /* move the buffer pointer to read the next char */
        }

      bytes_read = size - counter;  
    }

  else if (fd == 1)   // stdout
    {
      bytes_read = -1;
    }
  else    // user file
    {
      struct file_descriptor *file = get_file_by_fd(fd);
      if (file != NULL) 
        {
          bytes_read = file_read(file->file, buffer, size);
        }
    }

  lock_release(&file_lock);
  return bytes_read;  
}

int 
write (int fd, void* buffer, unsigned size)
{
  int bytes_written = 0;
  lock_acquire(&file_lock);

  if (fd == 0)
    {
       bytes_written = -1; 
    }
  else if (fd == 1)   // write to the console
    {
      putbuf(buffer, size);
      bytes_written = size;
    }
  else
    {
      struct file_descriptor *file = get_file_by_fd(fd);
      if (file != NULL) 
        {
          bytes_written = file_write(file->file, buffer, size);
        }
    }    

  lock_release(&file_lock);  
  return bytes_written;   
}

void 
seek (int fd, unsigned position)
{
  struct file_descriptor *file = get_file_by_fd(fd);
  if (file != NULL) 
    {
      lock_acquire(&file_lock);
      file_seek(file->file, position);
      lock_release(&file_lock);
    }
}

unsigned
tell (int fd)
{
  struct file_descriptor *file = get_file_by_fd(fd);
  int curr_pos = -1;
  if (file != NULL) 
    {
      lock_acquire(&file_lock);
      curr_pos = file_tell(file->file);
      lock_release(&file_lock);
    }
  return curr_pos;
  
}

void
close (int fd)
{
  struct file_descriptor *file;
  struct list *files = &thread_current() -> files;
  struct list_elem *e = list_begin(files);
  for(e; e != list_end(files); e = list_next(e))
    {
      file = list_entry (e, struct file_descriptor, elem);
      if (file -> fd == fd) 
        {
            list_remove(e); /* update thread's open files list */
            lock_acquire(&file_lock);
            file_close(file -> file);
            lock_release(&file_lock);
            break;
        }
    }
}

struct file_descriptor *
get_file_by_fd(int fd)
{
  struct file_descriptor *file;
  struct list *files = &thread_current() -> files;
  struct list_elem *e = list_begin(files);
  for(e; e != list_end(files); e = list_next(e))
    {
      file = list_entry (e, struct file_descriptor, elem);
      if (file -> fd == fd) 
            return file;
    }
  return NULL;
}

bool
is_valid_pointer(const void *ptr)
{
  return (ptr != NULL && is_user_vaddr(ptr));
}