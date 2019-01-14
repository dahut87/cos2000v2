/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "gdt.h"

#define E820_MAX_ENTRIES 128

typedef struct entrye820 {
	u64 addr;
	u64 size;
	u32 type;
} entrye820 __attribute__((packed));

typedef struct miniregs {
	union {
		struct {
			u32 eax;
			u32 ebx;
			u32 ecx;
			u32 edx;
			u32 esi;
			u32 edi;
			u32 ebp;
			u32 eflags;
		};
		struct {
			u16 ax, hax;
			u16 bx, hbx;
			u16 cx, hcx;
			u16 dx, hdx;
			u16 si, hsi;
			u16 di, hdi;
			u16 bp, hbp;
			u16 flags, hflags;
		};
		struct {
			u8 al, ah, hax2, hax3;
			u8 bl, bh, hbx2, hbx3;
			u8 cl, ch, hcx2, hcx3;
			u8 dl, dh, hdx2, hdx3;
		};
	};
} miniregs  __attribute__ ((packed));


#define STRINGIFY(x) #x
#define MACRO(x)     STRINGIFY(x)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))
#define __must_be_array(a) BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:(-!!(e)); }))

#define EFLAGS_CF			0x00000001
#define LOOPS_8042		100000
#define FF_8042			32
#define LOOPS_A20_ENABLE 	255
#define LOOPS_DELAY 		32768
#define SMAP	0x534d4150

#define interrupt(num, regs) ({\
	asm("mov %[eflagsregs],%%eax\n\
	    pushl %%eax\n\
	    popfl\n\
	    mov %[eaxregs],%%eax\n\
	    mov %[ebxregs],%%ebx\n\
	    mov %[ecxregs],%%ecx\n\
	    mov %[edxregs],%%edx\n\
	    mov %[esiregs],%%esi\n\
	    mov %[ediregs],%%edi\n\
          int %[numregs]\n\
	    pushfl\n\
          popl %%eax\n\
          mov %%eax,%[eflagsregs]\n\
	    mov %%eax,%[eaxregs]\n\
	    mov %%ebx,%[ebxregs]\n\
	    mov %%ecx,%[ecxregs]\n\
	    mov %%edx,%[edxregs]\n\
	    mov %%esi,%[esiregs]\n\
	    mov %%edi,%[ediregs]":[eaxregs] "+m" (regs.eax),[ebxregs] "+m" (regs.ebx),[ecxregs] "+m" (regs.ecx),[edxregs] "+m" (regs.edx),[esiregs] "+m" (regs.esi),[ediregs] "+m" (regs.edi),[eflagsregs] "+m" (regs.eflags):[numregs] "i" (num):"%eax","%ebx","%ecx","%edx","%esi","%edi");})
void cleanreg(miniregs *reg);
u8 initmemory(void);
void showchar(u8 achar);
void showstr(u8 *str);
u8 gettime(void);
u8 waitchar(void);
void initkeyboard(void);
u8 empty8042(void);
void iodelay(void);
u8 testA20(void);
void enableA20kbc(void);
void enableA20fast(void);
u8 enableA20(void);
void memset(void *dst, u8 val, u32 count, u32 size);
void memcpy(void *src, void *dst, u32 count, u32 size);
void initselectors(u32 executingoffset);
void makegdtdes(u32 base, u32 limite, u8 acces, u8 flags, gdtdes * desc);
void initgdt();
void maskinterrupts(void);
void initcoprocessor(void);
void initpmode(u32 offset);
void main(void);
