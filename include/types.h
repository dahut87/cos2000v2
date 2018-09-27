#ifndef ALLTYPES
# define ALLTYPES


typedef char            	int8;
typedef short           	int16;
typedef int             	int32;
typedef long long       	int64;

typedef char	      	        BYTE;
typedef short	      	        WORD;
typedef long	      	        DWORD;
typedef long long       	QWORD;

typedef signed char		SBYTE;
typedef signed short		SWORD;
typedef signed long		SDWORD;
typedef signed long long	SQWORD;

typedef unsigned char		UBYTE;
typedef unsigned short		UWORD;
typedef unsigned long		UDWORD;
typedef unsigned long long	UQWORD;

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
typedef unsigned long long int	u64;
typedef char	s8;
typedef short	s16;
typedef int	s32;
typedef long int s64;
typedef int bool; 

extern char ctype[];

#define true 1
#define false 0
#define NULL 0x0000

#define CT_UP	0x01	/* upper case */
#define CT_LOW	0x02	/* lower case */
#define CT_DIG	0x04	/* digit */
#define CT_CTL	0x08	/* control */
#define CT_PUN	0x10	/* punctuation */
#define CT_WHT	0x20	/* white space (space/cr/lf/tab) */
#define CT_HEX	0x40	/* hex digit */
#define CT_SP	0x80	/* hard space (0x20) */

/* without the cast to unsigned, DJGPP complains (using -Wall) */
#define isalnum(c)	((ctype + 1)[(unsigned)(c)] & (CT_UP | CT_LOW | CT_DIG))
#define isalpha(c)	((ctype + 1)[(unsigned)(c)] & (CT_UP | CT_LOW))
#define iscntrl(c)	((ctype + 1)[(unsigned)(c)] & (CT_CTL))
#define isdigit(c)	((ctype + 1)[(unsigned)(c)] & (CT_DIG))
#define isgraph(c)	((ctype + 1)[(unsigned)(c)] & (CT_PUN | CT_UP | CT_LOW | CT_DIG))
#define islower(c)	((ctype + 1)[(unsigned)(c)] & (CT_LOW))
#define isprint(c)	((ctype + 1)[(unsigned)(c)] & (CT_PUN | CT_UP | CT_LOW | CT_DIG | CT_SP))
#define ispunct(c)	((ctype + 1)[(unsigned)(c)] & (CT_PUN))
#define isspace(c)	((ctype + 1)[(unsigned)(c)] & (CT_WHT))
#define isupper(c)	((ctype + 1)[(unsigned)(c)] & (CT_UP))
#define isxdigit(c)	((ctype + 1)[(unsigned)(c)] & (CT_DIG | CT_HEX))
#define isascii(c)	((unsigned)(c) <= 0x7F)
#define toascii(c)	((unsigned)(c) & 0x7F)

#define tolower(c)	(isupper(c) ? c + 'a' - 'A' : c)
#define toupper(c)	(islower(c) ? c + 'A' - 'a' : c)

#endif
