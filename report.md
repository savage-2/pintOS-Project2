# Project 2: User Programs



## I. Group Members



- Shiyi Li 11610511@mail.sustech.edu.cn

- Mengwei Guo 11610615@mail.sustech.edu.cn

  

## II. Division of labor



- #### Task 1

  - **Code** : Li Shiyi
  - **Report **: Li Shiyi


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

![1558162981269](C:\Users\41313\AppData\Roaming\Typora\typora-user-images\1558162981269.png)



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

  #### *TODO*

  

- #### Relative Function

	syscall.c
  - shutdown_power_off()
  - exit_process(int status)
  - sys_exec(char *filename)
  - sys_practice()
	thread.c
  - thread_exit()
  - 



- #### Algorithm & implementation

  

   - **halt()**

    use shutdown_power_off() to close the system directly.

  - **exit()**

    get the return status of process from stack, delete the process from its parent's child_list and then allocate thread_exit(). In thread_exit(), as the current thread need to exit, its parent thread need to wait for it to exit until current thread exit finish.

    

  - **exec()**

    *TODO*

    

  - **practice()**

    *TODO*



- #### Synchronization

  ### *TODO*



### Task 3:  File Operation Syscalls





- #### Requirement Analysis

  In this task, we are required to implement several system calls about the file operation, which include **create (); remove ();  open ();  filesize ();  read ();  write (); seek ();  tell (); close ();** 

  

- #### Data structures

  #### *TODO*

  

- #### Relative Function

  #### *TODO*



- #### Algorithm & implementation

  

  - **create()**

    *TODO*

    

  - **remove()**

    *TODO*

    

  - **open()**

    *TODO*

    

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


    No, we don't modify the basic structure of default linked list. We add two linked list named **child_list**(child thread list of every thread) and **file_list**(the file description of one thread) in the thread.h by using the provided list manipulation.
