## API List 

APIs given by COS2000 libraries


###  LIBSYS

All fonctions in the "libsys" library.

------

`u32 getticks(void);`

*Description:Return the internal value of the timer*

* syscall id : **4**
* arguments : **0**
* results : **u32**
* dump of register cpu: **no**

------

`u32 testapi(u32 arg1, u32 arg2, u32 arg3);`

*Description:Simple function to test if SYSCALL API is correctly running*

* syscall id : **0**
* arguments : **3**
* * argument 1 : **u32 arg1** *first argument of your choice*
* * argument 2 : **u32 arg2** *second argument of your choice*
* * argument 3 : **u32 arg3** *third argument of your choice*
* results : **u32**
* dump of register cpu: **yes**

------

`u8 waitkey(void);`

*Description:Wait for user to press a key and return the ascii code pressed*

* syscall id : **1**
* arguments : **0**
* results : **u8**
* dump of register cpu: **no**

------

`u32 exit(u32 resultcode);`

*Description:End a task for user or kernel domain*

* syscall id : **5**
* arguments : **1**
* * argument 1 : **u32 resultcode** *Code result of the execution*
* results : **u32**
* dump of register cpu: **no**


###  LIBVIDEO

All fonctions in the "libvideo" library.

------

`u32 print(u8* string);`

*Description:Show a string on the screen*

* syscall id : **2**
* arguments : **1**
* * argument 1 : **u8* string** *string to show in ascii format*
* results : **u32**
* dump of register cpu: **no**


