#include "types.h"
#include "cpu.h"
#include "cpuid.h"
#include "memory.h"
#include "string.h"
#include "asm.h"

/* Technologies supportées */

static u8 *msg[] = {
	"mmx",
	"mmx2",
	"sse",
	"sse2",
	"sse3",
	"fpu",
	"3dNow!",
	"3dNow Extended!",
	"HyperThreading",
	"apic",
    "64bits",
    "syscall",
    "msr",
    "sse4a",
    "vmx",
    "sse41",
    "sse42",
    "apic2"
};

static u8 space[] = " ";

/******************************************************************************/

/* Annule les FLAGs CPU */

bool cansetflag(u32 flag)
{
	u32 r1, r2;
 asm("pushfl\n" "popl %0\n" "movl %0, %1\n" "xorl %2, %0\n" "pushl %0\n" "popfl\n" "pushfl\n" "popl %0\n" "pushl %1\n" "popfl\n": "=&r"(r1), "=&r"(r2):"ir"(flag));
	return ((r1 ^ r2) & flag) != 0;
}

/******************************************************************************/

/* Lance l'instruction CPUID */

void cpuid(u32 op, u32 * eax, u32 * ebx, u32 * ecx, u32 * edx)
{
 asm("cpuid": "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx): "a"(op):"cc");
}

/******************************************************************************/

/* Retourne les informations sur le CPU dans une structure cpuinfo */

u8 getcpuinfos(cpuinfo * proc)
{
	u32 i, maxfunction, maxextended, unused, regeax, regebx, regecx, regedx;
	bool *boolean;

	if (!cansetflag(0x00040000))
		return 1;	/*386 processor - no cpuid */
	if (!cansetflag(0x00200000))
		return 2;	/*486 processor with no cpuid */

	cpuid(0, &maxfunction, &unused, &unused, &unused);
	maxfunction &= 0xffff;
	cpuid(0x80000000, &maxextended, &unused, &unused, &unused);
	maxextended &= 0xffff;
	if (maxfunction >= 1) {
		cpuid(1, &regeax, &regebx, &regecx, &regedx);
		proc->stepping = (regeax & 0x0000000F);
		proc->models = ((regeax >> 4) & 0x0000000F);
		proc->family = ((regeax >> 8) & 0x0000000F);
		proc->types = ((regeax >> 12) & 0x00000003);
		proc->emodels = ((regeax >> 16) & 0x0000000F);
		proc->efamily = ((regeax >> 20) & 0x000000FF);

		proc->brandid = (regeax & 0xF);
		proc->linesize = ((regeax >> 8) & 0xF);
		proc->count = ((regeax >> 16) & 0xF);
		proc->apicid = ((regeax >> 24) & 0xF);

		proc->mmx = ((regedx >> 23) & 0x00000001);
		proc->sse = ((regedx >> 25) & 0x00000001);
		proc->sse2 = ((regedx >> 26) & 0x00000001);
		proc->sse3 = (regecx & 0x00000001);
		proc->fpu = (regedx & 0x00000001);
		proc->htt = ((regedx >> 28) & 0x00000001);
		proc->vmx = ((regecx >> 5) & 0x00000001);
		proc->sse41 = ((regecx >> 19) & 0x00000001);
		proc->sse42 = ((regecx >> 20) & 0x00000001);
		proc->apic2 = ((regecx >> 21) & 0x00000001);
	}
	if (maxextended >= 1) {
		cpuid(0x80000001, &regeax, &regebx, &regecx, &regedx);
		proc->mmx2 = ((regedx >> 22) & 0x00000001);
		proc->apic = ((regedx >> 9) & 0x00000001);
		proc->now3d = ((regedx >> 30) & 0x00000001);
		proc->now3d2 = ((regedx >> 31) & 0x00000001);
        proc->bits64 = ((regedx >> 29) & 0x00000001);
        proc->syscall = ((regedx >> 11) & 0x00000001);
        proc->msr = ((regedx >> 5) & 0x00000001);
        proc->sse4a = ((regecx >> 6) & 0x00000001);
	}
	if (maxextended >= 4) {
		int i;
		for (i = 0; i < 3; i++) {
			cpuid(0x80000002 + i, &regeax, &regebx, &regecx,
			      &regedx);
			memcpy(&regeax, &proc->detectedname[0 + i * 16], 4, 1);
			memcpy(&regebx, &proc->detectedname[4 + i * 16], 4, 1);
			memcpy(&regecx, &proc->detectedname[8 + i * 16], 4, 1);
			memcpy(&regedx, &proc->detectedname[12 + i * 16], 4, 1);
		}
	}
	boolean = &proc->mmx;
	i = 0;

	for (i = 0; i < 10; i++)
		if (*(boolean++) == 1) {
			strcat(msg[i], &proc->techs);
			strcat(space, &proc->techs);
		}
	return 0;
}

/******************************************************************************/

/* Retourne un élément de la pile */

u32 viewstack(u32 pointer)
{
	u32 stack = 0;
 asm("movl %[a1],%%ebp;" "movl (%%ebp), %[a1] ;": [result] "=r"(stack): [a1] "r"(pointer):"%ebp");
	return stack;
}

/******************************************************************************/

/* Affiche les registres CPU */

void dump_regs()
{
    cli();
	u32 eax = 0;
	u32 ebx = 0;
	u32 ecx = 0;
	u32 edx = 0;
	u32 esi = 0;
	u32 edi = 0;
	u32 ebp = 0;
	u32 esp = 0;
	u16 cs = 0;
	u16 ds = 0;
	u16 es = 0;
	u16 fs = 0;
	u16 gs = 0;
	u16 ss = 0;
	u32 cr0 = 0;
	u32 cr1 = 0;
	u32 cr2 = 0;
	u32 cr3 = 0;
	u32 cr4 = 0;
	u32 eflags = 0;
 asm("movl %%eax, %[a1] ;" "movl %%ebx, %[b1] ;" "movl %%ecx, %[c1] ;" "movl %%edx, %[d1] ;" "movl %%esi, %[e1] ;" "movl %%edi, %[f1] ;" "movl %%esp, %[g1] ;" "movl %%ebp, %[h1] ;" "movw %%cs, %[i1] ;" "movw %%ds, %[j1] ;" "movw %%es, %[k1] ;" "movw %%fs, %[l1] ;" "movw %%gs, %[m1] ;" "movw %%ss, %[n1] ;" "mov %%cr0, %%eax ;" "mov %%eax, %[o1] ;" "mov %%cr2, %%eax ;" "mov %%eax, %[p1] ;" "mov %%cr3, %%eax ;" "mov %%eax, %[q1] ;" "mov %%cr4, %%eax ;" "mov %%eax,%[r1] ;":
[a1] "=m"(eax),[b1] "=m"(ebx),[c1] "=m"(ecx),[d1] "=m"(edx),[e1] "=m"(esi),
[f1] "=m"(edi),[g1] "=m"(esp),[h1] "=m"(ebp),[i1] "=m"(cs),[j1] "=m"(ds),
[k1] "=m"(es),[l1] "=m"(fs),[m1] "=m"(gs),[n1] "=m"(ss), [o1] "=m"(cr0),[p1] "=m"(cr2),[q1] "=m"(cr3), [r1] "=m"(cr4));

	printf("EAX=%X EBX=%X ECX=%X EDX=%X\r\n",eax,ebx,ecx,edx);
	printf("ESI=%X EDI=%X ESP=%X EBP=%X\r\n",esi,edi,esp,ebp);
	printf("\033[1m CS=%hX  DS=%hX  ES=%hX  FS=%hX  GS=%hX  SS=%hX\033[0m\r\n",(u32)cs,(u32)ds,(u32)es,(u32)fs,(u32)gs,(u32)ss);
	printf("CR0=%X CR1=N/A       CR2=%X CR3=%X CR4=%X\r\n",cr0,cr2,cr3,cr4);

 asm("pushf      ;" "pop %[f1] ;":
[f1] "=m"(eflags));

	printf("EFLAGS=%X",eflags);

	if (eflags & (1 << 0))	// Carry
		printf(" (C1");
	else
		printf(" (C0");

	if (eflags & (1 << 2))	// Parity
		printf(" P1");
	else
		printf(" P0");

	if (eflags & (1 << 4))	// Adjust
		printf(" A1");
	else
		printf(" A0");

	if (eflags & (1 << 6))	// Zero
		printf(" Z1");
	else
		printf(" Z0");

	if (eflags & (1 << 7))	// Sign
		printf(" S1");
	else
		printf(" S0");

	if (eflags & (1 << 11))	// Overflow
		printf(" O1)\r\n");
	else
		printf(" O0)\r\n");

	printf("STACK\r\n");
    u32 i=0;
	for (u32 pointer = esp; pointer < 0x400000; pointer+=4) {
        if (pointer==ebp) print("\033[1m\033[31m");
		printf("+%d:%X\t\t%X\033[0m\033[37m\r\n", i++, pointer,viewstack(pointer));
        if (i>25) {
            print("...\r\n");
            break;
        }
    }
    sti();
}
