#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);
struct lock f_lock;

void
syscall_init (void) 
{
  lock_init(&f_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void is_kernel(const void *vaddr){
  if(is_kernel_vaddr(vaddr)) exit(-1);
}

void halt(void){
  shutdown_power_off ();
}

void exit(int status){
  printf("%s: exit(%d)\n", thread_name(), status);
  thread_current()->exit_status = status;
  thread_exit();
}

pid_t exec(const char *cmd_line){
  //싱크를 위한 수정사항
  // tid_t tid = process_execute(cmd_line);

  // if(tid != -1){
  //   struct thread * child_thread;
  //   struct list_elem * elem;
  //   for(elem=list_begin(&(thread_current()->child_thread)); elem!=list_end(&(thread_current()->child_thread));elem=list_next(elem)){
  //     child_thread=list_entry(elem, struct thread, child_thread_elem);
  //     if(tid == child_thread->tid){
  //       sema_down(&child_thread->test_sema);
  //       break;
  //     }
  //   }
  // }
  // return tid; 
  return process_execute(cmd_line);
}

int wait(pid_t pid){
  return process_wait(pid);
}

int read(int fd, void * buffer, unsigned int size){
  int res;
  uint8_t tmp;
  if(fd < 0 || fd == 1 || fd > 128) exit(-1);
  is_kernel(buffer);
  lock_acquire(&f_lock);
  if(fd == 0){
    for(res = 0; (res<size) && (tmp = input_getc()) ; res++){
      *(uint8_t*)(buffer+res) = tmp;
    }
  }
  else{
    struct file * f = process_get_file(fd);
    if(f == NULL){
      lock_release(&f_lock);
      exit(-1);
    }
    res = file_read(f,buffer,size);
  }
  lock_release(&f_lock);
  return res;
}

int write(int fd, const void * buffer, unsigned int size){
  int res;
  struct file * f;
  if(fd < 1 || fd > 128) exit(-1);
  is_kernel(buffer);
  lock_acquire(&f_lock);
  if(fd == 1){
    putbuf(buffer,size);
    lock_release(&f_lock);
    return size;
  } 
  else{
    f = process_get_file(fd);
    res = file_write(f,buffer,size);
    lock_release(&f_lock);
    return res;
  }
}

int fibonacci(int n){
  int dp[47];
  if(n == 0 || n ==1 ) return n;
  dp[0] = 0;
  dp[1] = 1;
  for(int i=2; i<=n ;i++){
    dp[i] = dp[i-1] + dp[i-2];
  }
  return dp[n];
}

int max_of_four_int(int a, int b, int c, int d){
  int m ,n;
  m = (a > b) ? a : b;
  n = (c > d) ? c : d;
  m = (m > n) ? m : n;
  //printf("max4 : %d\n",m);
  return m;
}

//create
bool create(const char *file, unsigned initial_size){
  if(file == NULL) exit(-1);
  return filesys_create(file,initial_size);
}
//remove
bool remove(const char * file){
  if(file == NULL) exit(-1);
  return filesys_remove(file);
}
//open
int open(const char * file){
  int fd;
  struct file * f;
  if(file == NULL) exit(-1);
  lock_acquire(&f_lock);
  f = filesys_open(file);
  if(f == NULL) {
    lock_release(&f_lock);
    return -1;
  }
  for(int i=3; i<130; i++){
    if(thread_current()->fd_table[i] == NULL){
      thread_current()->fd_table[i] = f;
      lock_release(&f_lock);
      return i;     
    }
  }
  lock_release(&f_lock);
  return -1;
}
//filesize
int filesize(int fd){
  struct file * f = process_get_file(fd);
  if(f==NULL) exit(-1);
  return file_length(f);
}
//seek
void seek(int fd, unsigned int position){
  struct file * f = process_get_file(fd);
  if(f==NULL) exit(-1);
  file_seek(f,position);
}
//tell
unsigned int tell(int fd){
  struct file * f = process_get_file(fd);
  if(f==NULL) exit(-1);
  return file_tell(f);
}
//close
void close(int fd){
  process_close_file(fd);
}
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  // printf ("system call!\n");
  // thread_exit ();
  switch(*(uint32_t *)(f->esp)){
    case SYS_HALT:
      halt();
      break;
    case SYS_EXIT:
      is_kernel(f->esp + 4);
      exit((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_EXEC:
      is_kernel(f->esp + 4);
      f->eax = exec((char *)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_WAIT:
      is_kernel(f->esp + 4);
      f->eax = wait((pid_t)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_CREATE:
      is_kernel(f->esp+4);
      is_kernel(f->esp+8);
      f->eax = create((char*)*(uint32_t*)(f->esp+4), *(uint32_t*)(f->esp+8));
      break;
    case SYS_REMOVE:
      is_kernel(f->esp+4);
      f->eax = remove((char*)*(uint32_t*)(f->esp+4));
      break;
    case SYS_OPEN:
      is_kernel(f->esp+4);
      f->eax = open((char*)*(uint32_t*)(f->esp+4));
      break;
    case SYS_FILESIZE:
      is_kernel(f->esp+4);
      f->eax = filesize(*(uint32_t*)(f->esp+4));
      break;
    case SYS_READ:
      is_kernel(f->esp + 4);
      is_kernel(f->esp + 8);
      is_kernel(f->esp + 12);
      f->eax = read((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*(uint32_t *)(f->esp + 12));
      break;
    case SYS_WRITE:
      is_kernel(f->esp + 4);
      is_kernel(f->esp + 8);
      is_kernel(f->esp + 12);
      f->eax = write((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*(uint32_t *)(f->esp + 12));
      break;
    case SYS_SEEK:
      is_kernel(f->esp+4);
      is_kernel(f->esp+8);
      seek((int)*(uint32_t*)(f->esp+4), (unsigned)*(uint32_t*)(f->esp+8));
      break;
    case SYS_TELL:
      is_kernel(f->esp+4);
      f->eax = tell((int)*(uint32_t*)(f->esp+4));
      break;
    case SYS_CLOSE:
      is_kernel(f->esp+4);
      close(*(uint32_t*)(f->esp+4));
      break;
    case SYS_FIBONACCI:
      is_kernel(f->esp + 4);
      f->eax = fibonacci((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_MAX_OF_FOUR_INT:
      is_kernel(f->esp + 4);
      is_kernel(f->esp + 8);
      is_kernel(f->esp + 12);
      is_kernel(f->esp + 16);
      f->eax = max_of_four_int((int)*(uint32_t *)(f->esp + 4), (int)*(uint32_t *)(f->esp + 8), 
        (int)*(uint32_t *)(f->esp + 12), (int)*(uint32_t *)(f->esp + 16));
      break;
  }
}
