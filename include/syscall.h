/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#define sysexit \
 asm volatile ("sysexit"::"c");


static inline long syscall(long syscall) {
long ret;
asm volatile (
"pushl %%ecx;\
pushl %%edx;\
mov %%esp,%%ecx;\
mov $1f,%%edx;\
sysenter;\
1:" : "=a" (ret) : "a" (syscall): "ecx","edx","memory");
return ret;
}

static inline long syscall1(long syscall, long arg1) {
long ret;
asm volatile (
"pushl %%ecx;\
pushl %%edx;\
mov %%esp,%%ecx;\
mov $1f,%%edx;\
sysenter;\
1:" : "=a" (ret) : "a" (syscall), "b" (arg1) : "ecx","edx","memory");
return ret;
}

static inline long syscall2(long syscall, long arg1, long arg2) {
long ret;
asm volatile (
"pushl %%ecx;\
pushl %%edx;\
mov %%esp,%%ecx;\
mov $1f,%%edx;\
sysenter;\
1:" : "=a" (ret) : "a" (syscall), "b" (arg1), "S" (arg2) : "ecx","edx","memory");
return ret;
}

static inline long syscall3(long syscall, long arg1, long arg2, long arg3) {
long ret;
asm volatile (
"pushl %%ecx;\
pushl %%edx;\
mov %%esp,%%ecx;\
mov $1f,%%edx;\
sysenter;\
1:" : "=a" (ret) : "a" (syscall), "b" (arg1), "S" (arg2), "D" (arg3) : "ecx","edx","memory");
return ret;
}

/* Vers 6 arguments maximum */
void initsyscall(void);
void sysenter_handler(void);
