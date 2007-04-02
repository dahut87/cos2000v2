#include "vga.h"

int main(void) {
	setvmode(0x1);
	clearscreen();
	setfont(1);
	print("Hello cos is here");
	while(1);
}