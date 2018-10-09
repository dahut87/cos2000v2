/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "cpu.h"
#include "memory.h"
#include "string.h"
#include "asm.h"
#include "gdt.h"

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

	for (i = 0; i < sizeof(msg); i++)
		if (*(boolean++) == 1) {
			strcat(msg[i], &proc->techs);
			strcat(space, &proc->techs);
		}
	return 0;
}

/******************************************************************************/
/* Retourne la tête de pile */

u32 getESP(void)
{
	u32 stack = 0;
    asm("movl %%esp,%[result];": [result] "=r"(stack));
	return stack;
}

/******************************************************************************/
/* Fixe la tête de pile */

u32 setESP(u32 stack)
{
    asm("movl %[param],%%esp;": [param] "=r"(stack));
}

/******************************************************************************/
/* Sauvegarde les registres CPU */

void dump_cpu(save_stack *stack)
{
asm("   movl %%eax,%%ebx":::);
asm("   addl %[size],%%esp \n \
        addl $0x8,%%esp \n \
        pushl %%eax \n \
        pushl %%ebx \n \
        pushl %%ecx \n \
        pushl %%edx \n \
        pushl %%esi \n \
        pushl %%edi \n \
        pushl %%ebp \n \
        pushl %%esp \n \
        pushl %%cs \n \
        pushl $0x0 \n \
        pushl %%ds \n \
        pushl %%es \n \
        pushl %%fs \n \
        pushl %%gs \n \
        pushl %%ss \n \
        pushf \n \
        mov %%eax,%%ebx \n \
        mov %%cr0, %%eax \n \
        pushl %%eax\n \
        mov %%cr2, %%eax \n \
        pushl %%eax\n \
        mov %%cr3, %%eax \n \
        pushl %%eax\n \
        mov %%cr4, %%eax \n \
        pushl %%eax \n \
        mov %%dr0, %%eax \n \
        pushl %%eax\n \
        mov %%dr1, %%eax \n \
        pushl %%eax\n \
        mov %%dr2, %%eax \n \
        pushl %%eax\n \
        mov %%dr3, %%eax \n \
        pushl %%eax\n \
        mov %%dr4, %%eax \n \
        pushl %%eax\n \
        mov %%dr5, %%eax \n \
        pushl %%eax\n \
        mov %%dr6, %%eax \n \
        pushl %%eax\n \
        mov %%dr7, %%eax \n \
        pushl %%eax\n \
        mov $0xC0000080, %%ecx \n \
        rdmsr \n \
        pushl %%edx \n \
        pushl %%eax \n \
        subl $0x8,%%esp \n \
        mov %%ebx,%%eax"::[size] "a" (sizeof(save_stack)):);
    save_stack new;
    memcpy(&new, stack, sizeof(save_stack), 1);
}

/******************************************************************************/
/* Affiche les registres CPU */

void show_lightcpu(save_stack *stack)
{
    u32 i;
	printf("\33[0mEAX=%Y EBX=%Y ECX=%Y EDX=%Y\r\n", stack->eax, stack->ebx, stack->ecx, stack->edx);
	printf("ESI=%Y EDI=%Y ESP=%Y EBP=%Y\r\n", stack->esi, stack->edi, stack->esp, stack->ebp);
	printf("EIP=%Y EFL=%Y [%c%c%c%c%c%c%c%c%c]\r\n", stack->eip, stack->eflags,
    (stack->eflags & (1 <<11)) ? 'O':'-',
    (stack->eflags & (1 <<10)) ? 'D':'-',
    (stack->eflags & (1 << 9)) ? 'I':'-',
    (stack->eflags & (1 << 8)) ? 'T':'-',
    (stack->eflags & (1 << 7)) ? 'S':'-',
    (stack->eflags & (1 << 6)) ? 'Z':'-',
    (stack->eflags & (1 << 4)) ? 'A':'-',
    (stack->eflags & (1 << 2)) ? 'P':'-',
    (stack->eflags & (1 << 0)) ? 'C':'-');
	printf("CS =%hY DS =%hY SS =%hY ES =%hY FS =%hY GS =%hY\r\n",stack->cs,stack->ds,stack->ss,stack->es,stack->fs,stack->gs);
    printf("CR0=%Y CR2=%Y CR3=%Y CR4=%Y\r\n\r\n\r\n",stack->cr0,stack->cr2,stack->cr3,stack->cr4);
    
    u8* size;
    u8* pointer;
    for(i=20;i<50;i++) {
        pointer=stack->eip-i;
        size=pointer;
        size+=50;
        while(pointer<size) {
            pointer+=decode(false, pointer,false);
            if (pointer==stack->eip) break;
        }
        if (pointer==stack->eip) break;
    }
    if (pointer==stack->eip)
        pointer=stack->eip-i;
    else
         pointer=stack->eip;       
    size=pointer;
    size+=50;
    while(pointer<size)
    {
        if (pointer==stack->eip)
            print("\33[41m\33[1m");
        else
            print("\33[40m\33[0m");           
         pointer+=disas(pointer); 
    }

	printf("\33[0m\r\n\r\n\r\nSTACK\r\n");
    if (abs(KERNEL_STACK_ADDR-stack->esp)>0x10000)
        printf("Pile invalide !");
    else
    {
        i=0;
	    for (u32 *pointer = stack->esp; pointer < KERNEL_STACK_ADDR; pointer ++) {
            if (i>0 && i % 10 == 0) print("\033[10A");
            if (i>=10)
                print("\033[25C");            
		    printf("+%d:%Y - %Y\r\n", i++, pointer, (u32)(*pointer));
        }
        for(u32 j=0;j<10-(i % 10);j++)
            print("\033[01B");
	}
}

/******************************************************************************/
/* Affiche les registres CPU */

void show_cpu(save_stack *stack)
{
	printf("EAX=%Y EBX=%Y ECX=%Y EDX=%Y\r\n", stack->eax, stack->ebx, stack->ecx, stack->edx);
	printf("ESI=%Y EDI=%Y ESP=%Y EBP=%Y\r\n", stack->esi, stack->edi, stack->esp, stack->ebp);
	printf("EIP=%Y EFL=%Y [%c%c%c%c%c%c%c%c%c]\r\n", stack->eip, stack->eflags,
    (stack->eflags & (1 <<11)) ? 'O':'-',
    (stack->eflags & (1 <<10)) ? 'D':'-',
    (stack->eflags & (1 << 9)) ? 'I':'-',
    (stack->eflags & (1 << 8)) ? 'T':'-',
    (stack->eflags & (1 << 7)) ? 'S':'-',
    (stack->eflags & (1 << 6)) ? 'Z':'-',
    (stack->eflags & (1 << 4)) ? 'A':'-',
    (stack->eflags & (1 << 2)) ? 'P':'-',
    (stack->eflags & (1 << 0)) ? 'C':'-');
	printf("CS =%hY %Y %Y DPL=%d %cS%d [%c%c%c] %h ub\r\n",stack->cs,getdesbase(stack->cs),getdeslimit(stack->cs),getdesdpl(stack->cs),getdestype(stack->cs),getdessize(stack->cs),getdesbit3(stack->cs),getdesbit2(stack->cs),getdesbit1(stack->cs),getdesalign(stack->cs));
	printf("DS =%hY %Y %Y DPL=%d %cS%d [%c%c%c] %h ub\r\n",stack->ds,getdesbase(stack->ds),getdeslimit(stack->ds),getdesdpl(stack->ds),getdestype(stack->ds),getdessize(stack->ds),getdesbit3(stack->ds),getdesbit2(stack->ds),getdesbit1(stack->ds),getdesalign(stack->ds));
	printf("SS =%hY %Y %Y DPL=%d %cS%d [%c%c%c] %h ub\r\n",stack->ss,getdesbase(stack->ss),getdeslimit(stack->ss),getdesdpl(stack->ss),getdestype(stack->ss),getdessize(stack->ss),getdesbit3(stack->ss),getdesbit2(stack->ss),getdesbit1(stack->ss),getdesalign(stack->ss));
	printf("ES =%hY %Y %Y DPL=%d %cS%d [%c%c%c] %h ub\r\n",stack->es,getdesbase(stack->es),getdeslimit(stack->es),getdesdpl(stack->es),getdestype(stack->es),getdessize(stack->es),getdesbit3(stack->es),getdesbit2(stack->es),getdesbit1(stack->es),getdesalign(stack->es));
	printf("FS =%hY %Y %Y DPL=%d %cS%d [%c%c%c] %h ub\r\n",stack->fs,getdesbase(stack->fs),getdeslimit(stack->fs),getdesdpl(stack->fs),getdestype(stack->fs),getdessize(stack->fs),getdesbit3(stack->fs),getdesbit2(stack->fs),getdesbit1(stack->fs),getdesalign(stack->fs));
	printf("GS =%hY %Y %Y DPL=%d %cS%d [%c%c%c] %h ub\r\n",stack->gs,getdesbase(stack->gs),getdeslimit(stack->gs),getdesdpl(stack->gs),getdestype(stack->gs),getdessize(stack->gs),getdesbit3(stack->gs),getdesbit2(stack->gs),getdesbit1(stack->gs),getdesalign(stack->gs));
    u32 tss;
    str(tss);
    printf("TR =%hY %Y %Y DPL=%d %cS%d [%c%c%c] %h ub\r\n",stack->gs,getdesbase(tss),getdeslimit(tss),getdesdpl(tss),getdestype(tss),getdessize(tss),getdesbit3(tss),getdesbit2(tss),getdesbit1(tss),getdesalign(tss));
	struct gdtr gdtreg;
	sgdt(&gdtreg);
     printf("GDT=     %Y %Y\r\n",gdtreg.base,gdtreg.limite);
	struct idtr idtreg;
	sidt(&idtreg);
     printf("IDT=     %Y %Y\r\n",idtreg.base,idtreg.limite);
     printf("CR0=%Y CR2=%Y CR3=%Y CR4=%Y\r\n",stack->cr0,stack->cr2,stack->cr3,stack->cr4);
     printf("DR0=%Y DR1=%Y DR2=%Y DR3=%Y\r\n",stack->dr0,stack->dr1,stack->dr2,stack->dr3);
     printf("DR4=%Y DR5=%Y DR6=%Y DR7=%Y\r\n",stack->dr4,stack->dr5,stack->dr6,stack->dr7);
     printf("EFER=%lY\r\n",stack->efer);
	printf("STACK\r\n");
    if (abs(KERNEL_STACK_ADDR-stack->esp)>0x10000)
        printf("Pile invalide !");
    else
    {
        u32 i=0;
	    for (u32 *pointer = stack->esp; pointer < KERNEL_STACK_ADDR; pointer ++) {
            if (i>0 && i % 10 == 0) print("\033[10A");
            if (i>=10)
                print("\033[25C");            
		    printf("+%d:%Y - %Y\r\n", i++, pointer, (u32)(*pointer));
        }
        for(u32 j=0;j<10-(i % 10);j++)
            print("\033[01B");
	}
}
/*******************************************************************************/
