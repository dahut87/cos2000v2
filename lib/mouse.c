/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "asm.h"
#include "mouse.h"
#include "keyboard.h"
#include "vga.h"
#include "video.h"

/******************************************************************************/
/* initialise la souris */

bool initmouse(void)
{
	u16     i = 1024;

	outkbd(0x64, 0xA8);	/* autorisé Aux */

	outmseack(0xf3);	/*sample */
	outmseack(100);		/*sample donnée */

	outmseack(0xe8);	/*resolution */
	outmseack(3);		/*resolution donnée */

	outmseack(0xe7);	/* echelle 2:1 */

	outmseack(0xf4);	/* Autorise peripherique */

	outmsecmd(0x47);	/* Autorise interruption */

	while (i-- > 0)
		if (inb(0x60) == 250)
			return 1;
	return 0;
}

static u8 bytepos = 0, mousereplies = 0;
static u8 mpacket[3];
static s32 mousex = 40, mousey = 12;
static u16 oldx = 0, oldy = 0;
static u8 oldchar = 0x00, oldattr = 0x07;
static bool mousebut1 = 0, mousebut2 = 0, mousebut3 = 0;
static u8 speed = 6;

/******************************************************************************/
/* ecriture vers souris */

void outmseack(u8 value)
{
	outkbd(0x64, 0xD4);
	outb(0x60, value);
	mousereplies++;
}

/* ecriture vers mode */

void outmsecmd(u8 command)
{
	outkbd(0x64, 0x60);
	outb(0x60, command);
}

/******************************************************************************/
/* Handler d'interruption de la souris IRQ 12 */

__attribute__((interrupt)) void mouse_handler(exception_stack_noerror *caller)
{
	u8      mbyte = inb(0x60);
	s8      changex, changey;

	if (mousereplies > 0)
	{
		if (mbyte == 0xFA)
		{
			mousereplies--;
			goto endofint;
		}
		mousereplies = 0;
	}

	mpacket[bytepos] = mbyte;
	bytepos++;

	if (bytepos == 3)
	{
		bytepos = 0;
		if (mpacket[1] == 0)
		{
			changex = 0;
		}
		else
		{
			changex =
				(mpacket[0] & 0x10) ? mpacket[1] -
				256 : mpacket[1];
		}
		if (mpacket[2] == 0)
		{
			changey = 0;
		}
		else
		{
			changey =
				-((mpacket[0] & 0x20) ? mpacket[2] -
				  256 : mpacket[2]);
		}

		mousex += (changex << speed);
		mousey += (changey << speed);

		if (mousex < 0)
		{
			mousex = 0;
		}
		if (mousex >= 65535)
		{
			mousex = 65535;
		}
		if (mousey < 0)
		{
			mousey = 0;
		}
		if (mousey >= 65535)
		{
			mousey = 65535;
		}
		videoinfos *info = getvideo_info();
		u16     newx = (u32) mousex * info->currentwidth / 65536;
		u16     newy = (u32) mousey * info->currentheight / 65536;

		// Retrieve mouse button status from packet
		mousebut1 = mpacket[0] & 1;
		mousebut2 = mpacket[0] & 2;
		mousebut3 = mpacket[0] & 4;

// printf("RX:%d\tRY:%d\tX:%d\tY:%d\tB1:%d\tB2:%d\tB3:%d\t\r\n",changex,changey,mousex,mousey,mousebut1,mousebut2,mousebut3);

		if (!info->isgraphic)
			showchar(newx, newy, 0xDB, 0x0F);
	}
      endofint:
	irqendmaster();
	irqendslave();
}

/*******************************************************************************/
