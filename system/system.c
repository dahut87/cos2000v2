#include "vga.h"
#include "video.h"
#include "idt.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "asm.h"
#include "cpu.h"
#include "string.h"

#include "ansi.c"

static cpuinfo cpu;   
static u8 noproc[]="\033[31mInconnu\033[0m\000";   

void ok()
{
static u8 okmsg[]="\033[99C\033[8D\033[37m\033[1m[  \033[32mOK\033[37m  ]\033[0m\000";
print(okmsg);
return;
}

int _main(void) {
                 
        cli();   
	      setvmode(0x02);
	    /*  Efface l'ecran   */
        print("\033[2J\000");    
        printf(ansilogo);
        print("\033[0mNoyau charge en memoire\000");
        ok();
        
        print("Initilisation de la table d'interruption\000"); 
        initidt();
        ok();
        
        print("Initialisation du controleur d'interruption\000");
        initpic();      
        sti();
        ok();

        print("Installation du handler timer\000");
        setidt((u32)timer, 0x20, INTGATE, 32);
        ok();
        
        print("Activation de l'IRQ 0\000"); 
        enableirq(0); 
        ok();
        
        print("Installation du handler clavier\000");
        setidt((u32)keyboard, 0x20, INTGATE, 33);
        ok();
        
        print("Activation de l'IRQ 1\000"); 
        enableirq(1);    
        ok(); 
                
        print("Installation du handler souris\000");
        setidt((u32)mouse, 0x20, INTGATE, 100);
        ok();
        
        print("Initialisation du controleur souris :\000");
        if (initmouse()==1)
        print(" \033[1m\033[32mSouris Presente\033[0m\000");
        else
        print(" \033[1m\033[31mSouris non detecte\033[0m\000");       
        ok();
        
        print("Activation de l'IRQ 2 (Controleur esclave)\000"); 
        enableirq(2);    
        ok();         
        
        print("Activation de l'IRQ 12\000"); 
        enableirq(12);    
        ok(); 
 
        strcpy(&noproc,&cpu.detectedname);
        getcpuinfos(&cpu); 
        
        printf("Processeur detecte\r Revision \t:%d\r Modele \t:%d\r Famille \t:%d\r Nom cpuid\t:%s\rJeux\t:%s\000",cpu.stepping,cpu.models,cpu.family,&cpu.detectedname,&cpu.techs);
        ok();
        u8 key=0;
        while(1)   
        {    
        key=waitascii();
        putchar(key);
        }

}

