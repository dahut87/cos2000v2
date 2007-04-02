#include "vga.h"
#include "video.h"
#include "idt.h"
#include "timer.h"
#include "keyboard.h"
#include "asm.h"

u8 printok[]="     \033[37m\033[1m[  \033[32mOK\033[37m  ]\033[0m\r\n";

int _main(void) {
        cli();
	      setvmode(0x02);
	      /* Efface l'ecran */
        print("\033[2J");
        print("Noyau charge en memoire");
        print(printok);
        print("Initilisation de la table d'interruption");
        initidt();
        print(printok);
        print("Initialisation du controleur d'interruption");
        initpic();      
        print(printok);
        print("Activation logicielle des interruptions");
        sti();
        print(printok);
        print("Installation du handler timer");
        setidt((u32)timer, 0x30, INTGATE, 32);
        print(printok);
        print("Activation de l'IRQ 0"); 
        enableirq(0); 
        print(printok);
         print("Installation du handler clavier");
        setidt((u32)keyboard, 0x30, INTGATE, 33);
        print(printok);
        print("Activation de l'IRQ 1"); 
        enableirq(1);    
        print(printok);      
        while(1);	
}

