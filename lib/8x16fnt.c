/* Police de caract�re fine 8x16 */
static u8 font8x16 [4096] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7E, 0x81, 0xA5, 0x81, 0x81, 0xBD, 0x99, 0x81, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7E, 0xFF, 0xDB, 0xFF, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x36, 0x7F, 0x7F, 0x7F, 0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x3C, 0x3C, 0xE7, 0xE7, 0xE7, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x7E, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
	0x00, 0x00, 0x0F, 0x07, 0x0D, 0x19, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3F, 0x33, 0x3F, 0x30, 0x30, 0x30, 0x70, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7F, 0x63, 0x7F, 0x63, 0x63, 0x63, 0x67, 0xE7, 0xE6, 0xC0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x18, 0xDB, 0x3C, 0xE7, 0x3C, 0xDB, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x40, 0x60, 0x70, 0x7C, 0x7F, 0x7C, 0x70, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x03, 0x07, 0x1F, 0x7F, 0x1F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7F, 0xDB, 0xDB, 0xDB, 0x7B, 0x1B, 0x1B, 0x1B, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3E, 0x63, 0x30, 0x1C, 0x36, 0x63, 0x63, 0x36, 0x1C, 0x06, 0x63, 0x3E, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x7E, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x7F, 0x06, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x7F, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x60, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x24, 0x66, 0xFF, 0x66, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x08, 0x1C, 0x1C, 0x3E, 0x3E, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x7F, 0x7F, 0x3E, 0x3E, 0x1C, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x24, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x42, 0x42, 0x42, 0xFF, 0x42, 0x42, 0x42, 0xFF, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x10, 0x7E, 0x90, 0x90, 0x90, 0x7C, 0x12, 0x12, 0x12, 0xFC, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x61, 0x91, 0x92, 0x64, 0x08, 0x10, 0x26, 0x49, 0x89, 0x86, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x70, 0x88, 0x88, 0x88, 0x50, 0x60, 0x91, 0x8A, 0x84, 0x4A, 0x31, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x18, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x06, 0x18, 0x20, 0x40, 0x40, 0x40, 0x40, 0x40, 0x20, 0x18, 0x06, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x60, 0x18, 0x04, 0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x10, 0x92, 0x54, 0x38, 0xFE, 0x38, 0x54, 0x92, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0xFE, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x10, 0x20, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x83, 0x85, 0x89, 0x91, 0xA1, 0xC1, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x01, 0x02, 0x3C, 0x40, 0x80, 0x80, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x01, 0x02, 0x3C, 0x02, 0x01, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x02, 0x06, 0x0A, 0x12, 0x22, 0x42, 0x82, 0xFF, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFF, 0x80, 0x80, 0x80, 0xFC, 0x02, 0x01, 0x01, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x80, 0xBC, 0xC2, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFF, 0x81, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x81, 0x42, 0x3C, 0x42, 0x81, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x81, 0x43, 0x3D, 0x01, 0x01, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x10, 0x20, 0x00, 0x00, 
	0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x38, 0x44, 0x82, 0x82, 0x04, 0x08, 0x10, 0x10, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x99, 0xA5, 0xA5, 0xA5, 0x9E, 0x80, 0x41, 0x3E, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x24, 0x42, 0x81, 0x81, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFC, 0x42, 0x41, 0x41, 0x42, 0x7C, 0x42, 0x41, 0x41, 0x42, 0xFC, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x81, 0x80, 0x80, 0x80, 0x80, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFC, 0x42, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x42, 0xFC, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFF, 0x80, 0x80, 0x80, 0x80, 0xFC, 0x80, 0x80, 0x80, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFF, 0x80, 0x80, 0x80, 0x80, 0xFC, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x81, 0x80, 0x9F, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x38, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x07, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x81, 0x82, 0x84, 0x88, 0x90, 0xE0, 0x90, 0x88, 0x84, 0x82, 0x81, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x81, 0xC3, 0xA5, 0x99, 0x99, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x81, 0xC1, 0xA1, 0x91, 0x89, 0x85, 0x83, 0x81, 0x81, 0x81, 0x81, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFC, 0x82, 0x81, 0x81, 0x82, 0xFC, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81, 0x89, 0x85, 0x42, 0x3D, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFC, 0x82, 0x81, 0x81, 0x82, 0xFC, 0x90, 0x88, 0x84, 0x82, 0x81, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x42, 0x81, 0x80, 0x40, 0x3C, 0x02, 0x01, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFE, 0x92, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x81, 0x81, 0x81, 0x81, 0x81, 0x42, 0x42, 0x24, 0x24, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x99, 0x99, 0xA5, 0xA5, 0x42, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x81, 0x81, 0x81, 0x42, 0x24, 0x18, 0x24, 0x42, 0x81, 0x81, 0x81, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x82, 0x82, 0x82, 0x82, 0x44, 0x28, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFF, 0x01, 0x01, 0x02, 0x04, 0x18, 0x20, 0x40, 0x80, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x7E, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x80, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x7E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x7E, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x24, 0x42, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 
	0x00, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x02, 0x02, 0x7E, 0x82, 0x82, 0x7D, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x80, 0x80, 0x80, 0x80, 0xBC, 0xC2, 0x81, 0x81, 0x81, 0xC2, 0xBC, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x80, 0x80, 0x80, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0x01, 0x01, 0x01, 0x3D, 0x43, 0x81, 0x81, 0x81, 0x43, 0x3D, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x81, 0xFF, 0x80, 0x40, 0x3E, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0E, 0x11, 0x10, 0x10, 0xFE, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x43, 0x81, 0x81, 0x43, 0x3D, 0x01, 0x02, 0x7C, 0x00, 0x00, 
	0x00, 0x80, 0x80, 0x80, 0x80, 0xBC, 0xC2, 0x81, 0x81, 0x81, 0x81, 0x81, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x08, 0x08, 0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x02, 0x02, 0x00, 0x06, 0x02, 0x02, 0x02, 0x02, 0x02, 0x82, 0x44, 0x38, 0x00, 0x00, 
	0x00, 0x80, 0x80, 0x80, 0x80, 0x82, 0x84, 0x88, 0x90, 0xA8, 0xC4, 0x82, 0x00, 0x00, 0x00, 0x00, 
	0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xEC, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xB8, 0xC4, 0x82, 0x82, 0x82, 0x82, 0x82, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xBC, 0xC2, 0x81, 0x81, 0x81, 0xC2, 0xBC, 0x80, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x43, 0x81, 0x81, 0x81, 0x43, 0x3D, 0x01, 0x01, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xBE, 0xC1, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x81, 0x80, 0x7E, 0x01, 0x81, 0x7E, 0x00, 0x00, 0x00, 0x00, 
	0x10, 0x10, 0x10, 0x10, 0x10, 0xFE, 0x10, 0x10, 0x10, 0x10, 0x11, 0x0E, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x81, 0x81, 0x81, 0x81, 0x43, 0x3D, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x81, 0x81, 0x81, 0x42, 0x24, 0x18, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x82, 0x92, 0x92, 0x92, 0xAA, 0x44, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x42, 0x24, 0x18, 0x24, 0x42, 0x81, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x81, 0x81, 0x81, 0x43, 0x3D, 0x01, 0x02, 0x7C, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x04, 0x18, 0x20, 0x40, 0xFF, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x04, 0x08, 0x10, 0x10, 0x10, 0x20, 0x10, 0x10, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x40, 0x20, 0x10, 0x10, 0x10, 0x08, 0x10, 0x10, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x70, 0x99, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x36, 0x63, 0x63, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1E, 0x33, 0x61, 0x60, 0x60, 0x61, 0x33, 0x1E, 0x06, 0x03, 0x3E, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x66, 0x66, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x06, 0x0C, 0x18, 0x00, 0x3E, 0x63, 0x7F, 0x60, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x08, 0x1C, 0x36, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x66, 0x66, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x30, 0x18, 0x0C, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1C, 0x36, 0x1C, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x60, 0x66, 0x3C, 0x0C, 0x06, 0x3C, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x08, 0x1C, 0x36, 0x00, 0x3E, 0x63, 0x7F, 0x60, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x66, 0x66, 0x00, 0x3E, 0x63, 0x7F, 0x60, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x30, 0x18, 0x0C, 0x00, 0x3E, 0x63, 0x7F, 0x60, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x66, 0x66, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x3C, 0x66, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x60, 0x30, 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x63, 0x63, 0x08, 0x1C, 0x36, 0x63, 0x63, 0x7F, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1C, 0x36, 0x1C, 0x00, 0x1C, 0x36, 0x63, 0x63, 0x7F, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0C, 0x18, 0x30, 0x00, 0x7F, 0x33, 0x30, 0x3E, 0x30, 0x33, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x6E, 0x3B, 0x1B, 0x7E, 0xD8, 0xDC, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1F, 0x36, 0x66, 0x66, 0x7F, 0x66, 0x66, 0x66, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x08, 0x1C, 0x36, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x63, 0x63, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x30, 0x18, 0x0C, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x3C, 0x66, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x30, 0x18, 0x0C, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x63, 0x63, 0x00, 0x63, 0x63, 0x63, 0x63, 0x3F, 0x03, 0x06, 0x3C, 0x00, 0x00, 0x00, 
	0x00, 0x63, 0x63, 0x1C, 0x36, 0x63, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x63, 0x63, 0x00, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x18, 0x7E, 0xC3, 0xC0, 0xC0, 0xC3, 0x7E, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1C, 0x36, 0x32, 0x30, 0x78, 0x30, 0x30, 0x30, 0x73, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xC3, 0x66, 0x3C, 0x18, 0xFF, 0x18, 0xFF, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xFC, 0x66, 0x66, 0x7C, 0x62, 0x66, 0x6F, 0x66, 0x66, 0xF3, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0E, 0x1B, 0x18, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x18, 0x18, 0xD8, 0x70, 0x00, 0x00, 0x00, 
	0x00, 0x0C, 0x18, 0x30, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0C, 0x18, 0x30, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0C, 0x18, 0x30, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0C, 0x18, 0x30, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3B, 0x6E, 0x00, 0x6E, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x3B, 0x6E, 0x00, 0x63, 0x73, 0x7B, 0x7F, 0x6F, 0x67, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3C, 0x6C, 0x6C, 0x3E, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x38, 0x6C, 0x6C, 0x38, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x60, 0xE0, 0x63, 0x66, 0x6C, 0x18, 0x30, 0x6E, 0xC3, 0x06, 0x0C, 0x1F, 0x00, 0x00, 0x00, 
	0x00, 0x60, 0xE0, 0x63, 0x66, 0x6C, 0x18, 0x33, 0x67, 0xCF, 0x1F, 0x03, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x3C, 0x3C, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1B, 0x36, 0x6C, 0x36, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x6C, 0x36, 0x1B, 0x36, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x00, 0x00, 
	0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x00, 0x00, 
	0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x18, 0xF8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xF6, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x18, 0xF8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0xF6, 0x06, 0xF6, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x06, 0xF6, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0xF6, 0x06, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x18, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x30, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x30, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0xF7, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xF7, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x30, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0xF7, 0x00, 0xF7, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x18, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xFF, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x18, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x6E, 0x6C, 0x6C, 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3E, 0x63, 0x7E, 0x63, 0x63, 0x7E, 0x60, 0x60, 0x20, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7F, 0x63, 0x63, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x7F, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7F, 0x63, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x63, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x6C, 0x6C, 0x6C, 0x6C, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3B, 0x6E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7E, 0x18, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1C, 0x36, 0x63, 0x63, 0x7F, 0x63, 0x63, 0x36, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x36, 0x36, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1E, 0x30, 0x18, 0x0C, 0x3E, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0xDB, 0xDB, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0x06, 0x7E, 0xDB, 0xDB, 0xF3, 0x7E, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1C, 0x30, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x30, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3E, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x18, 0x18, 0xFF, 0x18, 0x18, 0x18, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x30, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x30, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0E, 0x1B, 0x1B, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xD8, 0xD8, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3B, 0x6E, 0x00, 0x3B, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x38, 0x6C, 0x6C, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0F, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0xEC, 0x6C, 0x3C, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xD8, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x70, 0xD8, 0x30, 0x60, 0xC8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
