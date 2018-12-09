/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#define syscall0(syscall) ({ \
	u32 _v; \
	asm volatile (\
	"mov %%esp,%%ecx;\
	mov $1f,%%edx;\
	sysenter;\
	1:" : "=a" (_v) : "a" (syscall): "ecx","edx","memory"); \
	_v; \
})

#define syscall1(syscall,arg1) ({ \
	u32 _v; \
	asm volatile (\
	"mov %%esp,%%ecx;\
	mov $1f,%%edx;\
	sysenter;\
	1:" : "=a" (_v) : "a" (syscall), "b" (arg1) : "ecx","edx","memory"); \
	_v; \
})

#define syscall2(syscall,arg1,arg2) ({ \
	u32 _v; \
	asm volatile (\
	"mov %%esp,%%ecx;\
	mov $1f,%%edx;\
	sysenter;\
	1:" : "=a" (_v) : "a" (syscall), "b" (arg1), "S" (arg2) : "ecx","edx","memory"); \
	_v; \
})

#define syscall3(syscall,arg1,arg2,arg3) ({ \
	u32 _v; \
	asm volatile (\
	"mov %%esp,%%ecx;\
	mov $1f,%%edx;\
	sysenter;\
	1:" : "=a" (_v) : "a" (syscall), "b" (arg1), "S" (arg2), "D" (arg3)  : "ecx","edx","memory"); \
	_v; \
})

