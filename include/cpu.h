typedef struct cpuinfo
{	
u8 vendor[13];
u8 names[32];
u8 detectedname[48];
u8 techs[48];
u8 stepping;
u8 models;
u8 family;
u8 types;
u8 emodels;
u8 efamily;
u8 apicid;
u8 count;
u8 linesize;
u8 brandid;
bool mmx;
bool mmx2;
bool sse;
bool sse2;
bool sse3;
bool fpu;
bool now3d;
bool now3d2;
bool htt;
bool apic;
bool bits64;
bool syscall;
bool msr;
bool sse4a;
bool vmx;
bool sse41;
bool sse42;
bool apic2;
} cpuinfo  __attribute__ ((packed));	


u8 getcpuinfos(cpuinfo *inf); 

