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

    /* 0-halt */
    case SYS_HALT: {

    } 

    /* 1-exit */
    case SYS_EXIT: {
      printf ("system call!\n");
      thread_exit ();
    }

    /* 2-exec */
    case SYS_EXEC: {

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
