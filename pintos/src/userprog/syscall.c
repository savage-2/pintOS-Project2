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

    /* 0-halt : shut down the operating system*/
    case SYS_HALT: {
      /* shut the power off */
      shutdown_power_off ();
    } 

    /* 1-exit : kill the current thread */
    case SYS_EXIT: {
      /* print the message of exited thread */
      printf ("thread %s has exited!\n", thread_name());
      thread_exit ();
    }

    /* 2-exec : execute a new  */
    case SYS_EXEC: {

      const char *fileName = (char*)*(sp+1);

      /* if the filename is empty */
      if (fileName == NULL ) {
        printf ("thread %s has exited!\n", thread_name());
        thread_exit();
        break;
      }

      /* get the new thread by its tid */
      int newTid = process_execute(fileName);

      struct thread* newThread = thread_search(newTid);
          /* Block the parent thread until the child has executed or failed */

      /* first, block the parent thread
         when the child thread comes here, block it too
       */
      sema_down(&newThread->semaWS);

      /* parent thread get the states of child thread */
      f->eax = newThread->tid;

      /* wake up the child thread */
      sema_up(&newThread->semaWS);  
    }


    /* 3-wait */
    case SYS_WAIT: {

    }

    /* 4-practice 
    case SYS_PRACTICE: {

    } 
    */

    /* 5-create : create a new thread with initial size
       return value: true if successful, false otherwise */ 
    case SYS_CREATE: {
      
      const char *fileName = (char*)*(sp+1);

      /* if the filename is empty or too long, exit */
      if ( fileName == NULL | strlen(fileName) >= 14 ) {
         printf ("thread %s has exited!\n", thread_name());
         thread_exit();
         break;
      }

      unsigned fileSize = (unsigned)*(sp+2);

      /* create the file by the function in filesys.c
         the return value is the return value of filesys_create() */
      f->eax = filesys_create(fileName, fileSize);
      break;
    }


    /* 6-remove : remove the file based on its name 
       return value: true if successful, false otherwise */ 
    case SYS_REMOVE: {
      const char *fileName = (char*)*(sp+1);

      /* if the filename is empty or too long, exit */
      if ( fileName == NULL | strlen(fileName) >= 14 ) {
        thread_exit();
        break;
      } else {
        /* call filesys_remove and set its return value to return */
        f->eax = filesys_remove(fileName);
      }
    }


    /* 7-open : remove the file based on its name
       return value: the file descriptor if opened successfully, -1 otherwiase */
    case SYS_OPEN: {

      const char *fileName = (char*)*(sp+1);
      struct file *newFile;

      /* open the file and get its pointer by filesys_open() */
      newFile = filesys_open(fileName);

      /* if open failed, return -1 */
      if (newFile == NULL) {
        f->eax = -1;
        return;
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
      f->eax = fdValue;
      break;
    }


    /* 8-filesize : get the size of file in the indicated fd
       return : size of file */
    case SYS_FILESIZE: {

      int fd = *(sp+1);
      
      /* if the fd is invalid */
      if (fd < 0) {
        f->eax = 0;
        break;
      }

      /* find the file_description by its fd vlaue in current thread */
      struct file_description* file = thread_find_fd(thread_current(), fd);
      /* if the file isn't exist, break */
      if (file == NULL) {
        f->eax = 0;
        break;
      }

      /* get the lenght of file by calling file_length() */
      f->eax = file_length(file->f);
      break;
    }

    /* 9-read */
    case SYS_READ: {

    }


    /* 10-write : Writes size bytes from buffer to the open file fd.
       return: Returns the number of bytes actually written  */
    case SYS_WRITE: {
      
      /* get the fd value, buffer and buffer size */ 
      int fd = *(sp+1);
      const void *buffer = *(sp+2);
      int bufSize = *(sp+3);
      int writeSize;

      /* if the buffer is invalid, return 0 */
      if ( fd < 0 | buffer <= 0 ) {
        f->eax = 0;
        return;
      }

      /* if fd equals to 1, write to the console */
      if (fd == 1) {
        putbuf( (char*)buffer, bufSize);
        f->eax = bufSize;
      } 

      /* else, find the file with the aimed fd value and write buffer to it */
      struct file_description* writeFd = thread_find_fd(thread_current(), fd);
      if (writeFd == NULL) {
        return;
      } else {
        /* call the file_write function, return the size of written value */
        writeSize = file_write(writeFd->f, buffer, bufSize);
        f->eax = writeSize;
      }
      break;    
    }


    /* 11-seek : Changes the next byte to be read or written in open file fd to position 
        return : nothing */
    case SYS_SEEK: {
      int fd = *(sp+1);
      int position = *(sp+2);

      /* invalid fd value */
      if ( fd < 0) {
        break;
      }

      struct file_description* seekFd = thread_find_fd(thread_current(), fd);
      /* the fd itself cannot be NULL */
      if (seekFd == NULL) {
        break;
      } 
      /* the file in seekFd cannot be NULL*/
      if (seekFd->f == NULL) {
        break;
      }

      /* call file_seek() to seek the new position */
      file_seek(seekFd->f, position);
      break;
    }


    /* 12-tell : position of the next byte to be read or written in open file fd 
        return : position */
    case SYS_TELL: {
      int fd = *(sp+1);

      /* invalid fd value */
      if ( fd < 0) {
        break;
      }

      struct file_description* tellFd = thread_find_fd(thread_current(), fd);
      /* the fd itself cannot be NULL */
      if (tellFd == NULL) {
        break;
      } 
      /* the file in tellFd cannot be NULL*/
      if (tellFd->f == NULL) {
        break;
      }

      /* call file_tell() to get the position value */
      f->eax = file_tell(tellFd->f);

      break;
    }

    /* 13-close : close the opened file in the fd_list of current_thread base on file descriptor
      return : void  */
    case SYS_CLOSE: {

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
      break;
    }

    
  }

}
