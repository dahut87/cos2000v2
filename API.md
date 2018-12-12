## API List 

APIs given by COS2000 libraries

###  LIBC 

All fonction in the "libc" library.

------

`u32     libc_testapi(void);`

*Description: function to test if the syscall mecanism is running.*

* syscall id : **0**
* arguments : **0**
* results : **yes (always 0x66666666)**

------

`u32     libc_exit(u32 errorcode);`

*Description: tell system that the user process is now finish. Free all ressources affected.*

* syscall id : **1**
* arguments : **1**
* * argument 1 : **u32 pid** *PID process to free*
* results : **no**

------