# Project 2: User Programs



## I. Group Members



- Shiyi Li 11610511@mail.sustech.edu.cn

- Mengwei Guo 11610615@mail.sustech.edu.cn

  

## II. Division of labor



- #### Task 1

  - **Code** : Li Shiyi
  - **Report**: Li Shiyi

- #### Task 2 

  - **Code** : Li Shiyi & Guo Mengwei
  - **Report** : Li Shiyi & Guo Mengwei

- #### Task 3

  - **Code**: Li Shiyi & Guo Mengwei
  - **Report**: Li Shiyi & Guo Mengwei



## III. Implementation



*Before we started coding, we had a brief look at pintos itself. As the picture below shown,  while running a user program, pintos will follow the steps like:*

1. PintOS will  start and do the initialization, which includes storage allocation for kernel programs.
2. PintOS will load the ELF executable file and allocate address space for the user program (activate the page directory at the same time).
3. After that, the user program can take argument and push them in the stack of it. **It will be finished in Task 1.**
4. The System call function will read the parameters from the stack by moving the pointer. After executed the executable, it will put the return value in a specific position.
5. The Main process of operating system will wait for the return value of system call, when it has received the return value, the main process will be shut down.

![1558162981269](https://github.com/savage-2/pintOS-Project2/blob/master/1558162981269.png)



### Task 1: Argument Passing



- #### Requirement Analysis

  While executing an executable, it always requires arguments to control the running of the program. For example, the file name, the waiting time, and the path. The pintOS follows the style of parameter passing in C program, which is set two default parameters for the main function: **int argc** and **char* argv[].** argc generation the number of table arguments, and argv is an array of pointers passed in as strings. The placement of the parameters should be on the top of the stack of the user program stack. The stack, will expand space from high position to low position.

  

- #### Data structures

  #### *TODO*

  

- #### Relative Function

  #### *TODO*



- #### Algorithm & implementation

- **Get the name and path of executable**

  *TODO*

  

- **The time to pass the argument**

  After the  the **load()** function has finished in function **start_process()**, we can write the arguments to the top of stack. 

  

- **Decode the arguments string**

  Predefined function **strtok_r** to is used to separate out one argument (a string). The series of  argument strings will be copied to the user stack in preparation for the next step. It should be mentioned that the order of characters in string is opposite to the increasing direction of the stack.

  

  - **Set the value of pointer**

    After the program has been loaded, the executable's entry pointer eip will be set,as well as the initial stack pointer esp, which will point to the initial stack position by calling function setup_stack.



- #### Synchronization

  Everything in PintOS is represented by file, so while we run a specific syscall call, the user program shouldn't be modified (The file operation system call in Task 3 can do that) by other processes to prevent the changing of results. So we add a lock named **system_file_lock** in **thread.h** to implement the synchronization while loading the file.





### Task 2: Process Control Syscalls



- #### Requirement Analysis

  In this task, we are required to implement several system calls about the process control, which include **halt()**, **exit()**. **exec()** and **practice().** 

  

- #### Data structures

  ​         In ``struct thread``

  - ``bool load_success``: check whether child process load successfully.

  - ``semaphore child_lock``: record whether it need to wait for its child to execute.

  - ``int return_value``: status when a thread need to exit.

  - ``struct list *child_list``: store all its child thread.

  - ``struct thread *parent``: current thread's parent thread.

  - ``struct child_thread *waiting_child``: child thread that current thread is waiting for to execute.

    Add ``struct child_thread``

  - ``int tid``: tid of child thread.

  - ``struct list_elem child_elem``: elem of child thread.

  - ``int return_value``: return value of child thread.

  - ``bool is_waited``: whether child thread is been waited by its parent thread.

  - ``struct semaphore child_wait_lock``: whether child thread is waiting for other thread.

  

- #### Relative Function

  ​          **syscall.c**

  - sys_exit(int status): syscall of exit
  - sys_exec(char *filename): syscall of exec
  - sys_practice(): syscall of practice
    ​          **thread.c**
  - thread_exit(): exit a thread
    ​          **process.c**
  - check_addr(const void *vaddr): check whether the address in stack is valid

- #### Algorithm & implementation

  

​    

- **HALT**

Use ``shutdown_power_off()`` in device to close the system directly.

​    

- **EXIT: sys_exit()**

Get the return status of process from stack, delete the process from its parent's ``child_list`` and then allocate ``thread_exit()``. In ``thread_exit()``, as the current thread need to exit, its parent thread need to wait for it to exit until current thread exit finish.

​    

- **EXEC: sys_exec()**

Acquire ``system_file_lock`` first. Get filename from stack, if filename is null, which means thread has already exit, allocate ``thread_exit()`` to remove the thread. Else, use ``filesys_open()`` in ``filesys.h`` to open the file. If file exists, close the file, release the lock and allocate ``process_execute()`` in ``process.c``.

​    

- **PRACTICE: sys_practice()**

Acquire ``system_file_lock`` first. Get the parameter from stack and and add one on it. Then push it back to the stack and release the lock.
​  


- #### Synchronization

In ``sys_exec()`` and ``sys_practice()``, it need to operate on file system. In this two method, use ``lock_acquire(&system_file_lock)`` and ``lock_release(&system_file_lock)`` to prevent several processes operate on the same file.



### Task 3:  File Operation Syscalls



- #### Requirement Analysis

  In this task, we are required to implement several system calls about the file operation, which include **create (); remove ();  open ();  filesize ();  read ();  write (); seek ();  tell (); close ();** 

  

- #### Data structures

  ​         In ``struct thread``

  - ``struct list *file_list``: store all files that the current thread opens.
  - ``struct file *self``: current thread's executable file.
  - ``int file_count``: number of files it has opened.

  ​       Add ``struct file_describer``

  - ``int fd``: id of file_descirber.
  - ``struct list_elem file_elem``: elem of file_describer.
  - ``int file *ptr``: file of file_describer.

  

- #### Relative Function

  ​          **syscall.c**

  - syycall_handler(struct intr_frame *f): add syscall function.
  - sys_open(char *filename): open file with the filename.
  - sys_read(int size, void *buffer, int fd): read file with certain fd.
  - sys_write(int size, void *buffer, int fd): write into file with certain fd.
  - sys_close(int fd): close the file with its file_describer has certain fd.
    ​          **thread.c**
  - thread_exit(): exit a thread
    ​          **process.c**
  - check_addr(const void *vaddr): check whether the address in stack is valid



- #### Algorithm & implementation

  

  - **create()**

 Get the filename an size of the file that we want to create from stack and use ``check_addr`` to check whether these values are valid. Then acquire the ``system_file_lock`` to prevent other thread to operate on file system. Allocate ``filesys_create`` in ``filesys.h`` to create the file and release the lock.

​    

- **remove()**

  Get the filename of the file that we want to remove from stack and use ``check_addr`` to check whether these values are valid. Then allocate ``filesys_remove`` in ``filesys.h`` to remove the file.

​    

- **open()**

  Get the filename of the file that we want to remove from stack and use ``check_addr`` to check whether these values are valid. Then allocate ``sys_open`` to open file. In ``sys_open``, acquire the ``system_file_lock`` to prevent other thread to operate on file system. Allocate ``filesys_open`` in ``filesys.h`` to open the file and release the lock. If the file exists, use a file_describer of record the file and its fd in current thread. Then push it into current thread's ``file_list``.

​    

- **filesize()**

  *TODO*

  

- **read()**

  *TODO*

  

- **write()**

  *TODO*

  

- **seek()**

  *TODO*

  

- **tell()**

  *TODO*

  

- **close()**

  *TODO*

  

- #### Synchronization

  ### *TODO*





## IV.Addition questions



- #### *A reflection on the project–what exactly did each member do? What went well, and what could be improved?*

  The division of labor has been listed in the pervious part. **TODO**

  

- #### *Does your code exhibit any major memory safety problems (especially regarding C strings), memory leaks, poor error handling, or race conditions?*

  

- #### *Did you use consistent code style? Your code should blend in with the existing Pintos code. Check your use of indentation, your spacing, and your naming conventions.*



- #### *Is your code simple and easy to understand?*



- #### *If you have very complex sections of code in your solution, did you add enough comments to explain them?*



- #### *Did you leave commented-out code in your final submission?*



- #### *Did you copy-paste code instead of creating reusable functions?*

  No, we always try to create the reusable but not use duplicated code block in different places. For example , we create function xxxx and xxxx in thread.c to implement the file search in one thread. Because of that, we don't need to implement the same 

  

- #### *Are your lines of source code excessively long? (more than 100 characters)*

  No, there is no such type of line in our source code.

  

- #### *Did you re-implement linked list algorithms instead of using the provided list manipulation*?

```
No, we don't modify the basic structure of default linked list. We add two linked list named **child_list**(child thread list of every thread) and **file_list**(the file description of one thread) in the thread.h by using the provided list manipulation.
```
