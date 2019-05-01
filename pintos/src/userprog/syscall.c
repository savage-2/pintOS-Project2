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

      {
        /* data */
      };
      

    }


    /* 3-wait */
    case SYS_WAIT: {

    }

    /* 4-practice */
    case SYS_PRACTICE: {

    }

    /* 5-create */ 
    case SYS_CREATE: {

    }

    /* 6-remove */
    case SYS_REMOVE: {

    }

    /* 7-open */
    case SYS_OPEN: {

    }

    /* 8-filesize */
    case SYS_FILESIZE: {

    }

    /* 9-read */
    case SYS_READ: {

    }

    /* 10-write */
    case SYS_WRITE: {

    }

    /* 11-seek */
    case SYS_SEEK: {

    }

    /* 12-tell */
    case SYS_TELL: {

    }

    /* 13-close */
    case SYS_CLOSE: {

    }

    
  }

}
