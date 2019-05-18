#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "stdio.h"
#include "console.h"

static void syscall_handler (struct intr_frame *);
void sys_exit(struct intr_frame *f);
int sys_exec(struct intr_frame *f);
int sys_wait(struct intr_frame *f);
int sys_create(struct intr_frame *f);
int sys_remove(struct intr_frame *f);
int sys_open(struct intr_frame *f);
int sys_fileSize(struct intr_frame *f);
int sys_read(struct intr_frame *f);
int sys_write(struct intr_frame *f);
void sys_seek(struct intr_frame *f);
int sys_tell(struct intr_frame *f);
void sys_close(struct intr_frame *f);
void sys_halt(void);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{

  /* get the stack pointer of intr_frame  */
  uint32_t *sp = f->esp;

  /* choose user program based on the value of syscall number number */
  switch (*sp) {

    case SYS_HALT: 
      sys_halt(); 
      break;

		case SYS_EXIT: 
      sys_exit(f); 
      break;

		case SYS_EXEC: 
      f->eax = sys_exec(f); 
      break;

		case SYS_WAIT: 
      f->eax = sys_wait(f); 
      break;

		case SYS_CREATE: 
      f->eax = sys_create(f); 
      break;

		case SYS_REMOVE: 
      f->eax = sys_remove(f); 
      break;

		case SYS_OPEN: 
      f->eax = sys_open(f); 
      break;

		case SYS_FILESIZE: 
      f->eax = sys_fileSize(f); 
      break;

		case SYS_READ: 
      f->eax = sys_read(f); 
      break;

		case SYS_WRITE: 
      f->eax = sys_write(f); 
      break;

		case SYS_SEEK: 
      sys_seek(f); 
      break;

		case SYS_TELL: 
      f->eax = sys_tell(f); 
      break;

		case SYS_CLOSE: 
      sys_close(f); 
      break;

		default: 
      printf("Invalid system call!\n");

  }
}


/* 0-halt : shut down the operating system*/
void
sys_halt(void)
{
    /* shut the power off */
    shutdown_power_off ();   
}


/* 1-exit : kill the current thread */
void
sys_exit(struct intr_frame *f)
{
    /* print the message of exited thread */
    printf ("thread %s has exited!\n", thread_name());
    thread_exit ();
}


/* 2-exec : execute a new  */
int
sys_exec(struct intr_frame *f)
{

    /* get the stack pointer of intr_frame  */
    uint32_t *sp = f->esp;
    const char *fileName = (char*)*(sp+1);

    /* if the filename is empty */
    if (fileName == NULL ) {
      printf ("thread %s has exited!\n", thread_name());
      thread_exit();
    }

    /* get the new thread by its tid */
    int newTid = process_execute(fileName);

    struct thread* newThread = thread_search(newTid);
    
    /* Block the parent thread until the child has executed or failed 
        first, block the parent thread
        when the child thread comes here, block it too
      */
    sema_down(&newThread->semaWS);

    /* parent thread get the states of child thread */
    f->eax = newThread->tid;

    /* wake up the child thread */
    sema_up(&newThread->semaWS);  
    return newTid;

}


/* 3-wait */
int
sys_wait(struct intr_frame *f)
{

}


/* 4-create : create a new thread with initial size
   return value: true if successful, false otherwise */ 
int
sys_create(struct intr_frame *f)
{
    /* get the stack pointer of intr_frame  */
    uint32_t *sp = f->esp;
    const char *fileName = (char*)*(sp+1);

    /* if the filename is empty or too long, exit */
    if ( fileName == NULL | strlen(fileName) >= 14 ) {
        printf ("thread %s has exited!\n", thread_name());
        thread_exit();
    }

    unsigned fileSize = (unsigned)*(sp+2);

    /* create the file by the function in filesys.c
        the return value is the return value of filesys_create() */
    return filesys_create(fileName, fileSize);
    
}


/* 5-remove : remove the file based on its name 
   return value: true if successful, false otherwise */ 
int
sys_remove(struct intr_frame *f)
{

    /* get the stack pointer of intr_frame  */
    uint32_t *sp = f->esp;
    const char *fileName = (char*)*(sp+1);

    /* if the filename is empty or too long, exit */
    if ( fileName == NULL | strlen(fileName) >= 14 ) {
      return -1;
    } else {
      /* call filesys_remove and set its return value to return */
      return filesys_remove(fileName);
    }
    
}



 /* 6-open : remove the file based on its name
    return value: the file descriptor if opened successfully, -1 otherwiase */
int
sys_open(struct intr_frame *f)
{
    /* get the stack pointer of intr_frame  */
    uint32_t *sp = f->esp; 
    const char *fileName = (char*)*(sp+1);
    struct file *newFile;

    /* open the file and get its pointer by filesys_open() */
    newFile = filesys_open(fileName);

    /* if open failed, return -1 */
    if (newFile == NULL) {
      return -1;
    }

    /* get the right fd of new opened file */
    int fdValue = 0;
    struct thread* current  = thread_current();
    struct list* fd_list = &(current->fds);
    
    /* if the list is emppty */
    if ( list_empty(fd_list) ) {
      /* the 0 and 1 are reserved for the console, so decription started from 2 */
      fdValue = 2;

    } else {
      
      /* get the element in the fd list with the largset fd value */
      struct list_elem* MaxFd = list_max(fd_list, &fd_cmp, NULL);
      /* the new fd value will be max fa vlaue plus 1 */
      fdValue = list_entry(MaxFd, struct file_description, elem)->fd + 1;
    }

    /* allocate the space for new element in fd_list */
    struct file_description *newFd = (struct file_description *)malloc(sizeof(struct file_description));
    
    /* refresh the value in fd */
    newFd->fd = fdValue;
    newFd->f = newFile;
    list_push_front(fd_list, &newFd->elem);

    /* set return value as fd value of new opened file */
    return fdValue;
}


/* 7-filesize : get the size of file in the indicated fd
   return : size of file */
int
sys_fileSize(struct intr_frame *f)
{

    /* get the stack pointer of intr_frame  */
    uint32_t *sp = f->esp;
    int fd = *(sp+1);
    
    /* if the fd is invalid */
    if (fd < 0) {
      return 0;
    }

    /* find the file_description by its fd vlaue in current thread */
    struct file_description* file = thread_find_fd(thread_current(), fd);
    /* if the file isn't exist, break */
    if (file == NULL) {
      return 0;
    }

    /* get the lenght of file by calling file_length() */
    return file_length(file->f);
}



int
sys_read(struct intr_frame *f)
{

}



/* 9-write : Writes size bytes from buffer to the open file fd.
       return: Returns the number of bytes actually written  */
int
sys_write(struct intr_frame *f)
{
    /* get the stack pointer of intr_frame  */
    uint32_t *sp = f->esp;

    /* get the fd value, buffer and buffer size */ 
    int fd = *(sp+1);
    const void *buffer = *(sp+2);
    int bufSize = *(sp+3);
    int writeSize;

    /* if the buffer is invalid, return 0 */
    if ( fd < 0 | buffer <= 0 ) {
      return -1;
    }

    /* if fd equals to 1, write to the console */
    if (fd == 1) {
      putbuf( (char*)buffer, bufSize);
      return bufSize;
    } 

    /* else, find the file with the aimed fd value and write buffer to it */
    struct file_description* writeFd = thread_find_fd(thread_current(), fd);
    if (writeFd == NULL) {
      return -1; 
    } else {
      /* call the file_write function, return the size of written value */
      writeSize = file_write(writeFd->f, buffer, bufSize);
      return writeSize;
    }  
}



/* 10-seek : Changes the next byte to be read or written in open file fd to position 
    return : nothing */
void
sys_seek(struct intr_frame *f)
{
    /* get the stack pointer of intr_frame  */
    uint32_t *sp = f->esp;
    int fd = *(sp+1);
    int position = *(sp+2);

    /* invalid fd value */
    if ( fd < 0) {
      return;
    }

    struct file_description* seekFd = thread_find_fd(thread_current(), fd);
    
    /* the fd itself cannot be NULL */
    if (seekFd == NULL) {
      return;
    } 
    
    /* the file in seekFd cannot be NULL*/
    if (seekFd->f == NULL) {
      return;
    }

    /* call file_seek() to seek the new position */
    file_seek(seekFd->f, position);
}



/* 11-tell : position of the next byte to be read or written in open file fd 
   return : position */
int
sys_tell(struct intr_frame *f)
{
    
    /* get the stack pointer of intr_frame  */
    uint32_t *sp = f->esp;
    int fd = *(sp+1);

    /* invalid fd value */
    if ( fd < 0) {
      return -1;
    }

    struct file_description* tellFd = thread_find_fd(thread_current(), fd);
    /* the fd itself cannot be NULL */
    if (tellFd == NULL) {
      return -1;
    } 
    /* the file in tellFd cannot be NULL*/
    if (tellFd->f == NULL) {
      return -1;
    }

    /* call file_tell() to get the position value */
    return file_tell(tellFd->f);
}



/* 12-close : close the opened file in the fd_list of current_thread base on file descriptor
   return : void  */
void
sys_close(struct intr_frame *f)
{
    
    /* get the stack pointer of intr_frame  */
    uint32_t *sp = f->esp;
    int fd = *(sp+1);
    /* if the fd is wrong, return */
    if (fd < 0) {
      return;
    }
    
    /* find the file_description by its fd value */
    struct file_description* closeFd = thread_find_fd(thread_current(), fd);
    /* remove this file_description from the fd_list */
    thread_remove_fd(thread_current(), fd);

    /* close the file in this file_description */
    file_close(closeFd->f);
    
    /* free this fd at last */
    free(closeFd);
}
