/***********************************/
/* Librairie LDT.h                 */
/* 23 Mars 2007                    */
/***********************************/

#ifndef JREKCED_LDT
# define JREKCED_LDT

# include "type.h"

# define LDT_BLOC_SIZE  (128)        /* Nombre de descripteur LDT dans un bloc */

// Types d'acces
# define GLT_ACCESSED         (0x1)

# define LDT_DATA             (0x0)
# define LDT_DATA_WRITE       (0x2)
# define LDT_DATA_EXPAND      (0x4)

# define LDT_CODE             (0x8)
# define LDT_CODE_READ        (0xA)
# define LDT_CODE_CONFORMING  (0xD)

// Granularite
# define LDT_BYTE             (0x0)
# define LDT_4KB              (0x1)

// Mode du CPU
# define LDT_16BIT            (0x0)
# define LDT_32BIT            (0x1)


typedef struct
{
    UWORD Limite_0_15;      // Bit 00-15 : Limite (0-15)
    UWORD Adresse_0_15;     // Bit 16-31 : Adresse (0-15)
    UBYTE Adresse_16_23;    // Bit 32-39 : Adresse (16-23)
    UBYTE Acces : 1;        // 0 = System
                            // 1 = Code ou data
    UBYTE Type : 4;         // Bit 41-44 : Type
    UBYTE DPL : 2;          // Spécifie le niveau de privilège du segment
    UBYTE Present : 1;      // Indique si le segment est en mémoire (bit a 1) ou non (bit a 0)
    UBYTE Limite_16_19 : 4; // Bit 48-51 : Limite (16-19)
    UBYTE UserDefine : 1;   // Bit 52    : Utilisation libre
    UBYTE Unused : 1;       // Bit 53    : Non utilise (toujours = 0)
    UBYTE CPUMode : 1;      // Bit 54    : = 0 -> 16-Bit
                            //             = 1 -> 32-Bit
    UBYTE Granularite : 1;  // Si = 0 : Limite = 1 octet -> 1 Mo
                            // Si = 1 : Limite = 4 Ko -> 4 Go
    UBYTE Adresse_24_31;    // Bit 56-63 : Adresse (24-31)
} T_LDT;


typedef struct
{
   UWORD  Size;     // Taille de la GDT
   void * Adresse;  // Adresse phisique de la GDT;
   UWORD  Empty;

} T_LDT_reg;


// T_LDT *GDT_BASE = ((T_LDT*)0x0800L); /* Addresse physique ou reside la GDT, le 1e element est toujours null */


T_LDT * LDT_balloc (UWORD Size);       // Alloue un bloc de Size LDT

int     LDT_bfree  (void * Adresse);   // Libere un bloc LDT

T_LDT * LDT_brealloc (T_LDT * OldLDT, UWORD NewSize); // Redimensionne un bloc LDT, retourne NULL en cas d'erreur.

T_LDT * LDT_AllocLDT (T_LDT *BaseLDT); // Cherche un LDT libre dans un bloc precedement
                                       // alloue avec LDT_balloc.
                                       // Retourne NULL si aucun bloc disponible.
                                       // Si un bloc libre est trouve, il est marque comme
                                       // utilise et un pointeur sur ce LDT est retourne.

int LDT_FreeLDT (T_LDT *LDT); // Marque le LDT comme non utilise et le met a NULL.

int LDT_MapMemory (T_LDT *LDT, void * Linear,   // Met des donnees dans un LDT.
                   UDWORD Size,                 // Cette fonction est a tester.
                   UBYTE Granularite,
                   UBYTE DPL, UBYTE Type, 
                   UBYTE CPUMode);
#endif
