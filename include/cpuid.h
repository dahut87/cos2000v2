
inline bool cansetflag (u32 flag)
{
u32 r1, r2;
asm("pushfl\n"
"popl %0\n"
"movl %0, %1\n"
"xorl %2, %0\n"
"pushl %0\n"
"popfl\n"
"pushfl\n"
"popl %0\n"
"pushl %1\n"
"popfl\n"
: "=&r" (r1), "=&r" (r2)
: "ir" (flag)
);
return ((r1 ^ r2) & flag) != 0;
}

inline void cpuid(u32 op, u32 *eax, u32 *ebx,u32 *ecx, u32 *edx)
{
	asm("cpuid" : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)	: "a" (op) : "cc");
}
