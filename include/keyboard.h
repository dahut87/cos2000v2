#define	SCAN_CTRL		0x1D	
#define	SCAN_LEFTSHIFT		0x2A
#define	SCAN_RIGHTSHIFT	0x36
#define	SCAN_ALT		0x38
#define	SCAN_F1			    0x3B
#define	SCAN_F2			0x3C
#define	SCAN_F3			0x3D
#define	SCAN_F4			0x3E
#define	SCAN_F5			0x3F
#define	SCAN_F6			0x40
#define	SCAN_F7			0x41
#define	SCAN_F8			0x42
#define	SCAN_F9			0x43
#define	SCAN_F10		0x44
#define	SCAN_F11		0x57
#define	SCAN_F12		0x58
#define	SCAN_CAPSLOCK		0x3A
#define	SCAN_NUMLOCK		0x45
#define	SCAN_SCROLLLOCK	0x46

/* bit de statut
0x0100 est reservé pour les touches non-ASCII */
#define	STATUS_ALT	0x0200	
#define	STATUS_CTRL	0x0400	
#define	STATUS_SHIFT	0x0800
#define	STATUS_ANY	(STATUS_ALT | STATUS_CTRL | STATUS_SHIFT)
#define	STATUS_CAPS	0x1000	
#define	STATUS_NUM	0x2000	
#define	STATUS_SCRL	0x4000	

/* "ASCII" values for non-ASCII keys. All of these are user-defined.*/

#define	KEY_F1		0x80
#define	KEY_F2		(KEY_F1 + 1)
#define	KEY_F3		(KEY_F2 + 1)
#define	KEY_F4		(KEY_F3 + 1)
#define	KEY_F5		(KEY_F4 + 1)
#define	KEY_F6		(KEY_F5 + 1)
#define	KEY_F7		(KEY_F6 + 1)
#define	KEY_F8		(KEY_F7 + 1)
#define	KEY_F9		(KEY_F8 + 1)
#define	KEY_F10		(KEY_F9 + 1)
#define	KEY_F11		(KEY_F10 + 1)
#define	KEY_F12		(KEY_F11 + 1)
#define	KEY_INS		0x90
#define	KEY_DEL		(KEY_INS + 1)
#define	KEY_HOME	(KEY_DEL + 1)
#define	KEY_END		(KEY_HOME + 1)
#define	KEY_PGUP	(KEY_END + 1)
#define	KEY_PGDN	(KEY_PGUP + 1)
#define	KEY_LFT		(KEY_PGDN + 1)
#define	KEY_UP		(KEY_LFT + 1)
#define	KEY_DN		(KEY_UP + 1)
#define	KEY_RT		(KEY_DN + 1)
#define	KEY_PRNT	(KEY_RT + 1)
#define	KEY_PAUSE	(KEY_PRNT + 1)
#define	KEY_LWIN	(KEY_PAUSE + 1)
#define	KEY_RWIN	(KEY_LWIN + 1)
#define	KEY_MENU	(KEY_RWIN + 1)


void keyboard();
