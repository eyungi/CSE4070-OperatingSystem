#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
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
  return process_execute(cmd_line);
}

int wait(pid_t pid){
  return process_wait(pid);
}

int read(int fd, void * buffer, unsigned size){
  uint8_t tmp;
  if(fd == 0){
    int i;
    for( i=0; i<size && (tmp = input_getc()) ; i++){
      *(uint8_t*)(buffer+i) = tmp;
    }
    return i;
  }
}

int write(int fd, const void *buffer, unsigned size){
  if(fd == 1){
    putbuf(buffer,size);
    return size;
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
