/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hord� Nicolas             */
/*                                                                             */
#include "interrupts.h"
#include "types.h"
#include "asm.h"
#include "memory.h"
#include "keyboard.h"
#include "vga.h"
#include "video.h"

static u8 bufferscan[256] = { 0 };
static u8 bufferascii[256] = { 0 };

static u8 ptrscan = 0;
static u8 ptrascii = 0;
static u16 kbdstatus, breakcode;

/******************************************************************************/
/* Tables clavier */

static const u8 set1_normal[] = {
	0, 0x1B, '&', '�', '\"', '\'', '(', '-',
	'�', '_', '�', '�', ')', '=', '\b', '\t',
	'a', 'z', 'e', 'r', 't', 'y', 'u', 'i',
	'o', 'p', '^', '$', '\r', 0, 'q', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm',
	'�', '�', 0, '*', 'w', 'x', 'c', 'v',
	'b', 'n', ',', ';', ':', '!', 0, '*',
	0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0, 0, '<', 0,
	0
};

static const u8 set1_shift[] = {
	0, 0x1B, '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '�', '+', '\b', '\t',
	'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I',
	'O', 'P', '�', '�', '\r', 0, 'Q', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M',
	'%', 0, 0, '�', 'W', 'X', 'C', 'V',
	'B', 'N', '?', '.', '/', '�', 0, '*',
	0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0, 0, '>', 0,
	0
};

static const u8 set1_alt[] = {
	0, 0x1B, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, '\r', 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '*',
	0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0, 0, 0, 0,
	0
};

static const u8 set1_altgr[] = {
	0, 0x1B, 0, '~', '#', '{', '[', '|',
	'`', '\\', '^', '@', ']', '}', '\b', '\t',
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, '�', '\r', 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '*',
	0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0, 0, 0, 0,
	0
};

static const u8 set1_ctrl[] = {
	0, 0x1B, 0, 0, 0, 0, 0x1B, 0,
	0, 0x1C, 0, 0, 0x1D, 0, 0, 0x1F,
	0x01, 0x1A, 0x05, 0x012, 0x14, 0x19, 0x15, 0x09,
	0x0F, 0x10, 0x1E, 0, 0, 0, 0x11, 0x13,
	0x04, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0C, 0x0D,
	0, 0, 0, 0, 0x17, 0x18, 0x03, 0x16,
	0x02, 0x0E, 0, 0, 0, 0, 0, '*',
	0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0, 0, 0, 0,
	0
};

/******************************************************************************/

/* Attend une chaine de caract�re de taille max */

u8     *getstring(u8 * temp)
{
	u8      maxwidth = strlen(temp);
	u8     *pointer = temp;
	u8      ascii = 0;
	while (ascii != '\r')
	{
		ascii = waitascii();
		if (ascii == '\b' && pointer > temp)
		{
			pointer--;
			putchar(ascii);
		}
		else if (ascii > 31 && pointer <= temp + 80)
		{
			*pointer++ = ascii;
			putchar(ascii);
		}
	}
	*pointer = '\000';
	return temp;
}

/******************************************************************************/
/* Fonction qui attend l'appuie d'une touche g�n�rant un code ASCII puis le retourne */
/* SYSCALL 
{
"ID":1,
"LIBRARY":"libsys",
"NAME":"waitkey",
"INTERNALNAME":"waitascii",
"DESCRIPTION":"Wait for user to press a key and return the ascii code pressed",
"ARGS": [],
"RETURN":"u8"
}
END */

u8 waitascii(void)
{
	u8      oldptrascii = ptrascii;
	while ((oldptrascii == ptrascii));
	return bufferascii[ptrascii];
}

/******************************************************************************/
/* Envoi d'une commande vers le contr�leur de clavier */

void outkbd(u8 port, u8 data)
{
	u32     timeout;
	u8      state;

/* timeout */
	for (timeout = 500000L; timeout != 0; timeout--)
	{
		state = inb(0x64);
/* vide le buffer du 8042 */
		if ((state & 0x02) == 0)
			break;
	}
	if (timeout != 0)
		outb(port, data);
}

/******************************************************************************/
/* Redemarre l'ordinateur */

void reboot(void)
{
	u8      temp;
	cli();
/* vide le 8042 */
	do
	{
		temp = inb(0x64);
		if ((temp & 0x01) != 0)
		{
			(void) inb(0x60);
			continue;
		}
	}
	while ((temp & 0x02) != 0);
/* active le reset CPU */
	outb(0x64, 0xFE);
	while (1)
		/* boucle infinie */ ;
}

/******************************************************************************/
/* Converti un scancode vers une code ASCII */

unsigned convert(u32 keypressed)
{
	u8      temp, key, lastscan;
/* garde le dernier pointeur du buffer scan */
	lastscan = ptrscan;
/* incr�mente le pointeur est assigne au buffer le dernier scancode */
	ptrscan++;
	if (ptrscan == 255)
		ptrscan == 0;
	bufferscan[ptrscan] = keypressed;
/* break key (touche relach�) ? */
	if (keypressed >= 0x80)
		breakcode = 1;
	key = (keypressed & 0x7F);
/* Mise a jour des flags lors du relachement de touches de controle */
	if (breakcode)
	{
		if (key == SCAN_ALT)
		{
			kbdstatus &= ~STATUS_ALT;
			/* si ALT GR (E01D) alors activer aussi control */
			if (bufferscan[lastscan] == 0xE0)
				kbdstatus &= ~STATUS_CTRL;
		}
		else if (key == SCAN_CTRL)
			kbdstatus &= ~STATUS_CTRL;
		else if (key == SCAN_LEFTSHIFT || key == SCAN_RIGHTSHIFT)
			kbdstatus &= ~STATUS_SHIFT;
		breakcode = 0;
		return 0;
	}
/* Mise a jour des flags lors de l'appuie de touches de controle */
	if (key == SCAN_ALT)
	{
		kbdstatus |= STATUS_ALT;
		/* si ALT GR (E01D) alors desactiver aussi control */
		if (bufferscan[lastscan] == 0xE0)
			kbdstatus |= STATUS_CTRL;
		return 0;
	}
	else if (key == SCAN_CTRL)
	{
		kbdstatus |= STATUS_CTRL;
		return 0;
	}
	else if (key == SCAN_LEFTSHIFT || key == SCAN_RIGHTSHIFT)
	{
		kbdstatus |= STATUS_SHIFT;
		return 0;
	}

	else if ((key >= SCAN_F1) && (key <= SCAN_F8))
	{
		changevc(key - SCAN_F1);
	}

	else if (key == SCAN_F9)
	{
		regs    dump;
		show_cpu(&dump);
	}

	else if (key == SCAN_F10)
	{
		apply_nextvideomode();
	}

/* Scroll Lock, Num Lock, and Caps Lock mise a jour des leds */
	else if (key == SCAN_SCROLLLOCK)
	{
		kbdstatus ^= STATUS_SCRL;
		goto LEDS;
	}
	else if (key == SCAN_NUMLOCK)
	{
		kbdstatus ^= STATUS_NUM;
		goto LEDS;
	}
	else if (key == SCAN_CAPSLOCK)
	{
		kbdstatus ^= STATUS_CAPS;
	      LEDS:
		outkbd(0x60, 0xED);	/* "mise a jour des LEDS */
		temp = 0;
		if (kbdstatus & STATUS_SCRL)
			temp |= 1;
		if (kbdstatus & STATUS_NUM)
			temp |= 2;
		if (kbdstatus & STATUS_CAPS)
			temp |= 4;
		outkbd(0x60, temp);	/* 3 bits de poids faible pour les LEDs */
		return 0;
	}
/* Appuie de CRTL + ALT + SUPR ? */
	if ((kbdstatus & STATUS_CTRL) && (kbdstatus & STATUS_ALT)
	    && (key == 73))
	{
		print("redemarrage du systeme");
		reboot();
	}
/* est ce un code etendu */
	if ((bufferscan[lastscan] == 0xE0)
	    || ((kbdstatus & STATUS_NUM) && (key >= 0x47) && (key <= 0x53)
		&& (key != 0x4A) && (key != 0x4e)))
		/* exceptions */
	{
		/* '/' (E035) */
		if (key == 0x35)
			return '/';
		/* '\r' (E01C) 2�me enter num�rique */
		if (key == 0x1C)
			return '\r';
		/* 0x11 (E048) device control 1) */
		if (key == 0x48)
			return 0x11;
		/* 0x12 (E050) device control 2) */
		if (key == 0x50)
			return 0x12;
		/* 0x13 (E04b) device control 3) */
		if (key == 0x4b)
			return 0x13;
		/* 0x14 (E04d) device control 4) */
		if (key == 0x4d)
			return 0x14;
		/* 0x02 (E049) start of text) */
		if (key == 0x49)
			return 0x2;
		/* 0x03 (E051) end of text) */
		if (key == 0x51)
			return 0x3;
		/* 0x10 (E047) Line feed) */
		if (key == 0x47)
			return '\n';
		/* 0x1A (E052) Substitution) */
		if (key == 0x52)
			return 0x1A;
		/* 0x18 (E053) Cancel) */
		if (key == 0x53)
			return 0x18;
		/* 0x19 (E04f) End of medium) */
		if (key == 0x4f)
			return 0x19;
		return 0x00;
	}
	else
	{
/* detecte les SCANCODES invalides */
		if (key >= sizeof(set1_normal) / sizeof(set1_normal[0]))
			return 0;
/* converti le scancode en code ASCII en fonction du statut*/
		if (kbdstatus & STATUS_SHIFT || kbdstatus & STATUS_CAPS)
			temp = set1_shift[key];
		else if ((kbdstatus & STATUS_ALT)
			 && (kbdstatus & STATUS_CTRL))
			temp = set1_altgr[key];
		else if (kbdstatus & STATUS_CTRL)
			temp = set1_ctrl[key];
		else if (kbdstatus & STATUS_ALT)
			temp = set1_alt[key];
		else
			temp = set1_normal[key];
	}
/* si scancode non reconnu fin de fonction */
	if (temp == 0)
		return temp;
/* Renvoie le Code ascii */
	return temp;
}

/******************************************************************************/
/* Handler d'interruption IRQ 1 pour le clavier */

__attribute__((interrupt)) void keyboard_handler(exception_stack_noerror *caller)
{
	u8      scancode, ascii;
	while ((inb(0x64) & 1) == 0);
	scancode = inb(0x60);
	ascii = convert(scancode);
	if (ascii != 0)
	{
		ptrascii++;
		if (ptrascii == 255)
			ptrascii == 0;
		bufferascii[ptrascii] = ascii;
	}
	irqendmaster();
}

/******************************************************************************/
