/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
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

void    keyboard(void);
void    reboot(void);
void    outkbd(u8 port, u8 data);
u8      waitascii(void);
u8     *getstring(u8 * temp);
