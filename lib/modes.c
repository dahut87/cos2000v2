/* definition des registres de la carte VGA pour differents modes */

#define maxgraphmode 11
#define maxtextmode 5


static mode_def textmodes[maxtextmode] = {


	/*40*25 16 couleurs mode 0x00*/
	{
	0x67,
	0x03, 0x08, 0x03, 0x00, 0x02, 
	0x2D, 0x27, 0x28, 0x90, 0x2B, 0xA0, 0xBF, 0x1F, 0x00, 
	0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00, 
	0x9C, 0x8E, 0x8F, 0x14, 0x1F, 0x96, 0xB9, 0xA3, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00, 
	45, 25, 4
	},


	/*80*25 16 couleurs mode 0x01*/
	{ 
	0x67,
	0x03, 0x00, 0x03, 0x00, 0x02, 
	0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F, 0x00, 
	0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00, 
	0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00, 
	80, 25, 4
	},
	

	/*80*50 16 couleurs mode 0x02*/
	{
	0x63,
	0x03, 0x01, 0x03, 0x05, 0x02, 
	0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F, 0x00, 
	0x47, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x9C, 0x8E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x00, 0x00, 
	80, 50, 4
	}, 


	/*100*50 16 couleurs mode 0x03*/
	{
	0x67,
	0x03, 0x01, 0x03, 0x05, 0x02, 
	0x70, 0x63, 0x64, 0x85, 0x68, 0x84, 0xBF, 0x1F, 0x00, 
	0x47, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x9C, 0x8E, 0x8F, 0x32, 0x1F, 0x96, 0xB9, 0xA3, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x00, 0x00, 
	100, 50, 4
	}, 


	/*100*60 16 couleurs mode 0x04*/
	{
	0xA7,
	0x03, 0x01, 0x03, 0x05, 0x02, 
	0x70, 0x63, 0x64, 0x85, 0x68, 0x84, 0xFF, 0x1F, 0x00, 
	0x47, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0xE7, 0x8E, 0xDF, 0x32, 0x1F, 0xDF, 0xE5, 0xA3, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x00, 0x00, 
	100, 60, 4
	}
	}; 

static mode_def graphmodes[maxgraphmode] = {

	/*640*480 n&b mode 0x80*/
	{
	0xE3,
	0x03, 0x01, 0x0F, 0x00, 0x06,
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E, 0x00,
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0F, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x10, 0x11, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
 	0x01, 0x00, 0x0F, 0x00, 0x00,
	80, 60, 1
	},

	/*320*200 4 couleurs mode 0x81*/
	{
	0x63,
	0x03, 0x09, 0x03, 0x00, 0x02,
	0x2D, 0x27, 0x28, 0x90, 0x2B, 0x80, 0xBF, 0x1F, 0x00,
	0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x14, 0x00, 0x96, 0xB9, 0xA3, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x02, 0x00, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x10, 0x11, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
 	0x01, 0x00, 0x03, 0x00, 0x00,
	40, 25, 
	},

	/*640*480 16 couleurs mode 0x82*/
	{
	0xE3,
	0x03, 0x01, 0x0F, 0x00, 0x06,
	0x5F, 0x4F, 0x50, 0x82, 0x53, 0x9F, 0x0B, 0x3E, 0x00,
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xE9, 0x8B, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0F, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x10, 0x11, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00,
	80, 60, 4
	},

	/*720*480 16 couleurs mode 0x83*/
	{
	0xE7,
	0x03, 0x01, 0x08, 0x00, 0x06,
	0x6B, 0x59, 0x5A, 0x82, 0x60, 0x8D, 0x0B, 0x3E, 0x00,
	0x40, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x2D, 0x08, 0xE8, 0x05, 0xE3, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x10, 0x11, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00,
	90, 60, 4
	},

	/*800*600 16 couleurs mode 0x84*/
	{
	0xE7,
	0x03, 0x01, 0x0F, 0x00, 0x06,
	0x70, 0x63, 0x64, 0x92, 0x65, 0x82, 0x70, 0xF0, 0x00,
	0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x5B, 0x8C, 0x57, 0x32, 0x00, 0x58, 0x70, 0xE3, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0F, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x10, 0x11, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
 	0x01, 0x00, 0x0F, 0x00, 0x00,
	100, 75, 4
	},

	/*320*200 256 couleurs RAPIDE mode 0x85*/
	{
	0x63,
	0x03, 0x01, 0x0F, 0x00, 0x0E,
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x00,
	0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00,
	40, 25, 8
	},

	/*320*200 256 couleurs mode 0x86*/
	{
	0x63,
	0x03, 0x01, 0x0F, 0x00, 0x06, 
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x00, 
	0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x9C, 0x0E, 0x8F, 0x28, 0x00, 0x96, 0xB9, 0xE3, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00, 
	40, 25, 8
	}, 

	/*320*400 256 couleurs mode 0x87*/
	{
	0x63,
	0x03, 0x01, 0x0F, 0x00, 0x06, 
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x00, 
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x9C, 0x8E, 0x8F, 0x28, 0x00, 0x96, 0xB9, 0xE3, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00, 
	40, 50, 8
	},

	/*320*480 256 couleurs mode 0x88*/
	{ 
	0xE3,
	0x03, 0x01, 0x0F, 0x00, 0x06, 
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E, 0x00, 
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xEA, 0xAC, 0xDF, 0x28, 0x00, 0xE7, 0x06, 0xE3, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00, 
	40, 60, 8
	}, 

	/*360*480 256 couleurs mode 0x89*/
	{ 
	0xE7,
	0x03, 0x01, 0x0F, 0x00, 0x06, 
	0x6B, 0x59, 0x5A, 0x8E, 0x5E, 0x8A, 0x0D, 0x3E, 0x00, 
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xEA, 0xAC, 0xDF, 0x2D, 0x00, 0xE7, 0x06, 0xE3, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00, 
	45, 60, 8
	},

	/*400*600 256 couleurs mode 0x8A*/
	{  
	0xE7,
	0x03, 0x01, 0x0F, 0x00, 0x06, 
	0x74, 0x63, 0x64, 0x97, 0x68, 0x95, 0x86, 0xF0, 0x00, 
	0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x5B, 0x8D, 0x57, 0x32, 0x00, 0x60, 0x80, 0xE3, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00, 
	50, 75, 8
	}
	};

