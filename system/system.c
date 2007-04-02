#include "vga.h"

int __main(void) {
	setvmode(1);
	cls();
	print("Hello cos is here !");
	while(1);
}
