/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hord� Nicolas             */
/*                                                                             */

/* Police de caract�re fine 8x8 */
static u8 font8x8[2048] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x81, 0xA5,
		0x81,
	0xBD, 0x99, 0x81, 0x7E,
	0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E, 0x6C, 0xFE, 0xFE,
	0xFE, 0x7C, 0x38, 0x10, 0x00,
	0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00, 0x38, 0x7C,
	0x38, 0xFE, 0xFE, 0x7C, 0x38, 0x7C,
	0x10, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x7C, 0x00,
	0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00,
	0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF,
	0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00,
	0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3,
	0xFF, 0x0F, 0x07, 0x0F, 0x7D, 0xCC, 0xCC, 0xCC, 0x78,
	0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18,
	0x7E, 0x18, 0x3F, 0x33, 0x3F, 0x30, 0x30, 0x70, 0xF0, 0xE0,
	0x7F, 0x63, 0x7F, 0x63, 0x63,
	0x67, 0xE6, 0xC0, 0x99, 0x5A, 0x3C, 0xE7, 0xE7, 0x3C, 0x5A, 0x99,
	0x80, 0xE0, 0xF8, 0xFE,
	0xF8, 0xE0, 0x80, 0x00, 0x02, 0x0E, 0x3E, 0xFE, 0x3E, 0x0E, 0x02,
		0x00,
	0x18, 0x3C, 0x7E,
	0x18, 0x18, 0x7E, 0x3C, 0x18, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00,
		0x66, 0x00,
	0x7F, 0xDB,
	0xDB, 0x7B, 0x1B, 0x1B, 0x1B, 0x00, 0x3E, 0x63, 0x38, 0x6C, 0x6C,
		0x38, 0xCC, 0x78,
	0x00,
	0x00, 0x00, 0x00, 0x7E, 0x7E, 0x7E, 0x00, 0x18, 0x3C, 0x7E, 0x18,
		0x7E, 0x3C, 0x18, 0xFF,

	0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18,
		0x18, 0x7E, 0x3C, 0x18,
	0x00,
	0x00, 0x18, 0x0C, 0xFE, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x30, 0x60,
		0xFE, 0x60, 0x30,
	0x00, 0x00,
	0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x24, 0x66,
		0xFF, 0x66,
	0x24, 0x00, 0x00,
	0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF,
		0x7E,
	0x3C, 0x18, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x00, 0x40, 0x00,
	0x90, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x50,
	0xF8, 0x50, 0xF8, 0x50, 0x50, 0x00,
	0x20, 0x78, 0xA0, 0x70, 0x28, 0xF0, 0x20, 0x00, 0xC8,
	0xC8, 0x10, 0x20, 0x40, 0x98, 0x98, 0x00,
	0x70, 0x88, 0x50, 0x20, 0x54, 0x88, 0x74, 0x00,
	0x60, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x20, 0x40, 0x80, 0x80, 0x80, 0x40, 0x20,
	0x00, 0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20, 0x00,
	0x00, 0x20, 0xA8, 0x70, 0x70, 0xA8,
	0x20, 0x00, 0x00, 0x00, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x60, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x60, 0x60, 0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
		0x00,
	0x70, 0x88, 0x98,
	0xA8, 0xC8, 0x88, 0x70, 0x00, 0x40, 0xC0, 0x40, 0x40, 0x40, 0x40,
		0xE0, 0x00,
	0x70, 0x88,
	0x08, 0x10, 0x20, 0x40, 0xF8, 0x00, 0x70, 0x88, 0x08, 0x10, 0x08,
		0x88, 0x70, 0x00,
	0x08,
	0x18, 0x28, 0x48, 0xFC, 0x08, 0x08, 0x00, 0xF8, 0x80, 0x80, 0xF0,
		0x08, 0x88, 0x70, 0x00,

	0x20, 0x40, 0x80, 0xF0, 0x88, 0x88, 0x70, 0x00, 0xF8, 0x08, 0x10,
		0x20, 0x40, 0x40, 0x40,
	0x00,
	0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00, 0x70, 0x88, 0x88,
		0x78, 0x08, 0x08,
	0x70, 0x00,
	0x00, 0x00, 0x60, 0x60, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x60,
		0x60, 0x00,
	0x60, 0x60, 0x20,
	0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x00, 0x00, 0x00, 0xF8,
		0x00,
	0xF8, 0x00, 0x00, 0x00,
	0x80, 0x40, 0x20, 0x10, 0x20, 0x40, 0x80, 0x00, 0x78, 0x84, 0x04,
	0x08, 0x10, 0x00, 0x10, 0x00,
	0x70, 0x88, 0x88, 0xA8, 0xB8, 0x80, 0x78, 0x00, 0x20, 0x50,
	0x88, 0x88, 0xF8, 0x88, 0x88, 0x00,
	0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0xF0, 0x00, 0x70,
	0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00,
	0xF0, 0x88, 0x88, 0x88, 0x88, 0x88, 0xF0, 0x00,
	0xF8, 0x80, 0x80, 0xE0, 0x80, 0x80, 0xF8, 0x00,
	0xF8, 0x80, 0x80, 0xE0, 0x80, 0x80, 0x80,
	0x00, 0x70, 0x88, 0x80, 0x80, 0x98, 0x88, 0x78, 0x00,
	0x88, 0x88, 0x88, 0xF8, 0x88, 0x88,
	0x88, 0x00, 0xE0, 0x40, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00,
	0x38, 0x10, 0x10, 0x10, 0x10,
	0x90, 0x60, 0x00, 0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x88, 0x00,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0xF8, 0x00, 0x82, 0xC6, 0xAA, 0x92, 0x82, 0x82, 0x82,
		0x00,
	0x84, 0xC4, 0xA4,
	0x94, 0x8C, 0x84, 0x84, 0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88,
		0x70, 0x00,
	0xF0, 0x88,
	0x88, 0xF0, 0x80, 0x80, 0x80, 0x00, 0x70, 0x88, 0x88, 0x88, 0xA8,
		0x90, 0x68, 0x00,
	0xF0,
	0x88, 0x88, 0xF0, 0xA0, 0x90, 0x88, 0x00, 0x70, 0x88, 0x80, 0x70,
		0x08, 0x88, 0x70, 0x00,

	0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x88, 0x88, 0x88,
		0x88, 0x88, 0x88, 0x70,
	0x00,
	0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x20, 0x00, 0x82, 0x82, 0x82,
		0x82, 0x92, 0x92,
	0x6C, 0x00,
	0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00, 0x88, 0x88, 0x88,
		0x50, 0x20,
	0x20, 0x20, 0x00,
	0xF8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xF8, 0x00, 0xE0, 0x80, 0x80,
		0x80,
	0x80, 0x80, 0xE0, 0x00,
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0xE0, 0x20, 0x20,
	0x20, 0x20, 0x20, 0xE0, 0x00,
	0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF8, 0x00,
	0x40, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x70, 0x08, 0x78, 0x88, 0x74, 0x00,
	0x80, 0x80, 0xB0, 0xC8, 0x88, 0xC8, 0xB0, 0x00,
	0x00, 0x00, 0x70, 0x88, 0x80, 0x88, 0x70, 0x00,
	0x08, 0x08, 0x68, 0x98, 0x88, 0x98, 0x68,
	0x00, 0x00, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x70, 0x00,
	0x30, 0x48, 0x40, 0xE0, 0x40, 0x40,
	0x40, 0x00, 0x00, 0x00, 0x34, 0x48, 0x48, 0x38, 0x08, 0x30,
	0x80, 0x80, 0xB0, 0xC8, 0x88,
	0x88, 0x88, 0x00, 0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x70, 0x00,
	0x10, 0x00, 0x30, 0x10,
	0x10, 0x10, 0x90, 0x60, 0x80, 0x80, 0x88, 0x90, 0xA0, 0xD0, 0x88,
		0x00,
	0xC0, 0x40, 0x40,
	0x40, 0x40, 0x40, 0xE0, 0x00, 0x00, 0x00, 0xEC, 0x92, 0x92, 0x92,
		0x92, 0x00,
	0x00, 0x00,
	0xB0, 0xC8, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x70, 0x88, 0x88,
		0x88, 0x70, 0x00,
	0x00,
	0x00, 0xB0, 0xC8, 0xC8, 0xB0, 0x80, 0x80, 0x00, 0x00, 0x68, 0x98,
		0x98, 0x68, 0x08, 0x08,

	0x00, 0x00, 0xB0, 0xC8, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x78,
		0x80, 0x70, 0x08, 0xF0,
	0x00,
	0x40, 0x40, 0xE0, 0x40, 0x40, 0x50, 0x20, 0x00, 0x00, 0x00, 0x88,
		0x88, 0x88, 0x98,
	0x68, 0x00,
	0x00, 0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, 0x00, 0x00, 0x82,
		0x82, 0x92,
	0x92, 0x6C, 0x00,
	0x00, 0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x88,
		0x88,
	0x98, 0x68, 0x08, 0x70,
	0x00, 0x00, 0xF8, 0x10, 0x20, 0x40, 0xF8, 0x00, 0x10, 0x20, 0x20,
	0x40, 0x20, 0x20, 0x10, 0x00,
	0x40, 0x40, 0x40, 0x00, 0x40, 0x40, 0x40, 0x00, 0x40, 0x20,
	0x20, 0x10, 0x20, 0x20, 0x40, 0x00,
	0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x00,
	0x3E, 0x60, 0xC0, 0x60, 0x3E, 0x08, 0x04, 0x18,
	0x00, 0x48, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0x76,
	0x18, 0x20, 0x00, 0x78, 0xCC, 0xFC, 0xC0,
	0x78, 0x10, 0x28, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x76,
	0x00, 0x48, 0x00, 0x78, 0x0C, 0x7C,
	0xCC, 0x76, 0x30, 0x08, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x76,
	0x48, 0x30, 0x00, 0x78, 0x0C,
	0x7C, 0xCC, 0x76, 0x78, 0xCC, 0xC0, 0xCC, 0x78, 0x10, 0x08, 0x30,
	0x30, 0x48, 0x84, 0x78,
	0xCC, 0xFC, 0xC0, 0x78, 0x00, 0x48, 0x00, 0x78, 0xCC, 0xFC, 0xC0,
		0x78,
	0x30, 0x08, 0x00,
	0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00, 0x48, 0x00, 0x30, 0x30, 0x30,
		0x30, 0x30,
	0x30, 0x48,
	0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x60, 0x10, 0x00, 0x30, 0x30,
		0x30, 0x30, 0x30,
	0x48,
	0x00, 0x30, 0x78, 0xCC, 0xCC, 0xFC, 0xCC, 0x30, 0x48, 0x30, 0x48,
		0x84, 0xFC, 0x84, 0x84,

	0x18, 0x20, 0x00, 0xF8, 0x80, 0xF0, 0x80, 0xF8, 0x00, 0x00, 0x00,
		0x66, 0x19, 0x77, 0x88,
	0x77,
	0x00, 0x00, 0x00, 0x0F, 0x14, 0x3E, 0x44, 0x87, 0x30, 0x48, 0x84,
		0x78, 0xCC, 0xCC,
	0xCC, 0x78,
	0x00, 0x48, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x60, 0x10, 0x00,
		0x78, 0xCC,
	0xCC, 0xCC, 0x78,
	0x30, 0x48, 0x84, 0x00, 0xCC, 0xCC, 0xCC, 0x76, 0x60, 0x10, 0x00,
		0xCC,
	0xCC, 0xCC, 0xCC, 0x76,
	0x48, 0x00, 0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8, 0x44, 0x00, 0x38,
	0x6C, 0xC6, 0xC6, 0x6C, 0x38,
	0x24, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x08,
	0x1C, 0x28, 0x28, 0x1C, 0x08, 0x00,
	0x1C, 0x22, 0x20, 0x70, 0x20, 0x22, 0x5C, 0x00, 0x44,
	0x28, 0x10, 0x10, 0x38, 0x10, 0x38, 0x10,
	0xF0, 0x88, 0x8A, 0xF7, 0x82, 0x82, 0x83, 0x00,
	0x06, 0x08, 0x08, 0x3C, 0x10, 0x10, 0x60, 0x00,
	0x18, 0x20, 0x00, 0x78, 0x0C, 0x7C, 0xCC,
	0x76, 0x18, 0x20, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x18, 0x20, 0x00, 0x78, 0xCC, 0xCC,
	0xCC, 0x78, 0x18, 0x20, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0x76,
	0x80, 0x78, 0x04, 0xF8, 0xCC,
	0xCC, 0xCC, 0xCC, 0x80, 0x7E, 0x01, 0xC6, 0xE6, 0xD6, 0xCE, 0xC6,
	0x00, 0x78, 0x0C, 0x7C,
	0xCC, 0x76, 0x00, 0xFE, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x00,
		0xFC,
	0x00, 0x00, 0x18,
	0x18, 0x30, 0x60, 0x66, 0x3C, 0xFF, 0x80, 0x80, 0x80, 0x00, 0x00,
		0x00, 0x00,
	0xFF, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0xC4, 0x48, 0x50, 0x26,
		0x49, 0x82, 0x07,
	0x40,
	0xC4, 0x48, 0x50, 0x26, 0x4A, 0x9F, 0x02, 0x00, 0x30, 0x00, 0x30,
		0x30, 0x30, 0x30, 0x30,

	0x00, 0x12, 0x24, 0x48, 0x90, 0x48, 0x24, 0x12, 0x00, 0x48, 0x24,
		0x12, 0x09, 0x12, 0x24,
	0x48,
	0x49, 0x00, 0x92, 0x00, 0x49, 0x00, 0x92, 0x00, 0x6D, 0x00, 0xB6,
		0x00, 0x6D, 0x00,
	0xB6, 0x00,
	0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0x10, 0x10, 0x10,
		0x10, 0x10,
	0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
		0xF0,
	0x10, 0xF0, 0x10, 0x10,
	0x28, 0x28, 0x28, 0x28, 0xE8, 0x28, 0x28, 0x28, 0x00, 0x00, 0x00,
	0x00, 0xF8, 0x28, 0x28, 0x28,
	0x00, 0x00, 0x00, 0xF0, 0x10, 0xF0, 0x10, 0x10, 0x28, 0x28,
	0x28, 0xE8, 0x08, 0xE8, 0x28, 0x28,
	0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x00,
	0x00, 0x00, 0xF8, 0x08, 0xE8, 0x28, 0x28,
	0x28, 0x28, 0x28, 0xE8, 0x08, 0xF8, 0x00, 0x00,
	0x28, 0x28, 0x28, 0x28, 0xF8, 0x00, 0x00, 0x00,
	0x10, 0x10, 0x10, 0xF0, 0x10, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x1F, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF,
	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x10, 0x10, 0x10,
	0x00, 0x00, 0x00, 0x00,
	0xFF, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0xFF, 0x10, 0x10,
		0x10,
	0x10, 0x10, 0x10,
	0x1F, 0x10, 0x1F, 0x10, 0x10, 0x28, 0x28, 0x28, 0x28, 0x3F, 0x28,
		0x28, 0x28,
	0x28, 0x28,
	0x28, 0x2F, 0x20, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x20,
		0x2F, 0x28, 0x28,
	0x28,
	0x28, 0x28, 0xEF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
		0x00, 0xEF, 0x28, 0x28,

	0x28, 0x28, 0x28, 0x2F, 0x20, 0x2F, 0x28, 0x28, 0x00, 0x00, 0x00,
		0xFF, 0x00, 0xFF, 0x00,
	0x00,
	0x28, 0x28, 0x28, 0xEF, 0x00, 0xEF, 0x28, 0x28, 0x10, 0x10, 0x10,
		0xFF, 0x00, 0xFF,
	0x00, 0x00,
	0x28, 0x28, 0x28, 0x28, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFF, 0x00,
	0xFF, 0x10, 0x10,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
		0x28,
	0x3F, 0x00, 0x00, 0x00,
	0x10, 0x10, 0x10, 0x1F, 0x10, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1F, 0x10, 0x1F, 0x10, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0x28, 0x28, 0x28, 0x28, 0x28,
	0x28, 0x28, 0xFF, 0x28, 0x28, 0x28,
	0x10, 0x10, 0x10, 0xFF, 0x10, 0xFF, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0xF0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
	0x00, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x34,
	0x4C, 0x4C, 0x32, 0x00, 0x5C, 0x22, 0x22, 0x3C, 0x44, 0x44, 0x78,
	0x7E, 0x42, 0x42, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x02, 0x7C, 0xA8, 0x28, 0x28,
		0x44,
	0x00, 0x7E, 0x61,
	0x30, 0x18, 0x08, 0x10, 0x20, 0x00, 0x00, 0x08, 0x7F, 0x88, 0x88,
		0x88, 0x70,
	0x00, 0x00,
	0x00, 0x22, 0x44, 0x44, 0x7A, 0x80, 0x00, 0x00, 0x00, 0x7C, 0x10,
		0x10, 0x10, 0x10,
	0x00,
	0x1C, 0x08, 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00, 0x38, 0x44,
		0x44, 0x7C, 0x44, 0x44,

	0x3C, 0x66, 0xC3, 0xC3, 0xC3, 0x66, 0x24, 0x66, 0x0C, 0x10, 0x08,
		0x1C, 0x22, 0x22, 0x22,
	0x1C,
	0x00, 0x00, 0x00, 0x00, 0x6C, 0x92, 0x92, 0x6C, 0x00, 0x01, 0x1A,
		0x26, 0x2A, 0x32,
	0x2C, 0x40,
	0x00, 0x18, 0x20, 0x20, 0x30, 0x20, 0x20, 0x18, 0x00, 0x3C, 0x42,
		0x42, 0x42,
	0x42, 0x42, 0x42,
	0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x08, 0x08,
		0x3E,
	0x08, 0x08, 0x00, 0x3E,
	0x00, 0x10, 0x08, 0x04, 0x08, 0x10, 0x00, 0x3E, 0x00, 0x04, 0x08,
	0x10, 0x08, 0x04, 0x00, 0x3E,
	0x00, 0x06, 0x09, 0x09, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00,
	0x08, 0x08, 0x08, 0x48, 0x48, 0x30,
	0x00, 0x00, 0x08, 0x00, 0x3E, 0x00, 0x08, 0x00, 0x00,
	0x60, 0x92, 0x0C, 0x60, 0x92, 0x0C, 0x00,
	0x60, 0x90, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x30, 0x78, 0x30, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
	0x00, 0x00, 0x03, 0x04, 0x04, 0xC8, 0x28, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x7C, 0x42, 0x42,
	0x42, 0x00, 0x18, 0x24, 0x08, 0x10, 0x3C, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3E, 0x3E,
	0x3E, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
