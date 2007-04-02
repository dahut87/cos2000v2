#include "vga.h"
#include "video.h"

int _main(void) {
	setvmode(0x84);
        cls();
        int x,y;
        for(x=0;x<120;x++)
        for(y=0;y<120;y++)
        {
       	writepxl(x,y,4);
        }
        for(x=0;x<20000;x++)
        {
        print("C'est 2 test 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789 c un test!");
	}
        while(1);	
}
