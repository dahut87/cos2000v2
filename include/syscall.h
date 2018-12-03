/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#define sysexit \
 asm volatile ("sysexit"::"c");


static inline long syscall(long syscall) {
long ret;
asm volatile (
"mov %%esp,%%ecx;\
mov $1f,%%edx;\
sysenter;\
1:" : "=a" (ret) : "a" (syscall): "ecx","edx","memory");
return ret;
}

static inline long syscall1(long syscall, long arg1) {
long ret;
asm volatile (
"mov %%esp,%%ecx;\
mov $1f,%%edx;\
sysenter;\
1:" : "=a" (ret) : "a" (syscall), "b" (arg1) : "ecx","edx","memory");
return ret;
}

static inline long syscall2(long syscall, long arg1, long arg2) {
long ret;
asm volatile (
"mov %%esp,%%ecx;\
mov $1f,%%edx;\
sysenter;\
1:" : "=a" (ret) : "a" (syscall), "b" (arg1), "S" (arg2) : "ecx","edx","memory");
return ret;
}

static inline long syscall3(long syscall, long arg1, long arg2, long arg3) {
long ret;
asm volatile (
"mov %%esp,%%ecx;\
mov $1f,%%edx;\
sysenter;\
1:" : "=a" (ret) : "a" (syscall), "b" (arg1), "S" (arg2), "D" (arg3) : "ecx","edx","memory");
return ret;
}

/*static inline long syscall4(long syscall, long arg1, long arg2, long arg3, long arg4) {
long ret;
asm volatile ("mov %%esp,%%ecx;
"mov $1f,%%edx;
"sysenter;
1:" : "=a" (ret) : "a" (syscall), "b" (arg1), "c" (arg2), 
"d" (arg3), "S" (arg4) : “memory”);
return ret;
}

static inline long syscall5(long syscall, long arg1, long arg2, long arg3, long arg4, long arg5) {
long ret;
asm volatile ("mov %%esp,%%ecx;
"mov $1f,%%edx;
"sysenter;
1:" : "=a" (ret) : "a" (syscall), "b" (arg1), "c" (arg2), 
"d" (arg3), "S" (arg4), "D" (arg5) : “memory”);
return ret;
}

static inline long syscall6(long syscall, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
long ret;
asm volatile ("mov %%esp,%%ecx;
"mov $1f,%%edx;
"sysenter;
1:" : "=a" (ret) : "a" (syscall), "b" (arg1), "c" (arg2), 
"d" (arg3), "S" (arg4), "D", (arg5), "d" (arg6) : “memory”);
return ret;
}*/
void initsyscall(void);
void sysenter_handler(void);
