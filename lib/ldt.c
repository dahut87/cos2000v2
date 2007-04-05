/***********************************/
/* Librairie LDT.c                 */
/* 23 Mars 2007                    */
/***********************************/

#include "LDT.h"
#include "Kmemory.h"

# define LDT_MANAGER_SIZE  (127)

typedef struct // Structure de gestion des bloc LDT
{
   void   * Next;                      // Pointeur de chainage
   UDWORD  Empty;
   T_LDT_reg LDTdesc [LDT_MANAGER_SIZE]; // Nombre de blocs LDT
} T_LDT_Manager;

void            LDT_initalloc (void *AllocTable, UDWORD Index, UDWORD SetSize, UDWORD TotalSize);
T_LDT_Manager * LDT_mballoc (UWORD Size);

/////////////////////////////////////////////////////////////////////////

T_LDT_Manager * LDT_Manager = NULL;

/////////////////////////////////////////////////////////////////////////

T_LDT * LDT_balloc (UWORD Size)  // Alloue un bloc de Size LDT
{
    UWORD Boucle;
    UDWORD Blocs, Boucle2;
    T_LDT_Manager * * ptr_Manager = &LDT_Manager;
    T_LDT_Manager *   Manager     = LDT_Manager;

    do
    {
       if (* ptr_Manager == NULL)
       {
          * ptr_Manager = Kernel_mballoc (1);
          Manager = * ptr_Manager;
          Manager->Next = NULL;
          for (Boucle = 0; Boucle < LDT_MANAGER_SIZE; Boucle++)
          {
             Manager->LDTdesc [Boucle].Adresse = NULL;
             Manager->LDTdesc [Boucle].Size    = NULL;
          }
       }

       for (Boucle = 0; Boucle < LDT_MANAGER_SIZE; Boucle++)
       {
          if (Manager->LDTdesc [Boucle].Adresse == NULL)
          {

             Blocs = Size * sizeof(T_LDT);
             if (Blocs % MEMORY_BLOC_SIZE == 0)
             { Blocs /= MEMORY_BLOC_SIZE; } else { Blocs = (Blocs / MEMORY_BLOC_SIZE) + 1; }
             Manager->LDTdesc [Boucle].Adresse = Kernel_mballoc (Blocs);
             if (Manager->LDTdesc [Boucle].Adresse == NULL) return NULL;
             Manager->LDTdesc [Boucle].Size    = (Blocs * MEMORY_BLOC_SIZE) / sizeof(T_LDT);
             
             // Mise a zero de la LDT
             for (Boucle2 = 0; Boucle2 < Manager->LDTdesc [Boucle].Size; Boucle2++)
             { ((int64*) Manager->LDTdesc [Boucle].Adresse)[Boucle2] = NULL; }

             return Manager->LDTdesc [Boucle].Adresse;
          }
       }

       *ptr_Manager = Manager;
       Manager      = Manager->Next;
    } while (1);
}


int LDT_bfree (void * Adresse) // Libere un bloc LDT
{
    UWORD  Boucle, Boucle2;
    int    RetVal;
    T_LDT_Manager * * ptr_Manager = &LDT_Manager;
    T_LDT_Manager *   Manager     = LDT_Manager;

    do
    {
       if (*ptr_Manager == NULL) { return -1; }
       for (Boucle = 0; Boucle < LDT_MANAGER_SIZE; Boucle++)
       {
          if (Manager->LDTdesc [Boucle].Adresse == Adresse)
          {

             RetVal = Kernel_mbfree(Manager->LDTdesc [Boucle].Adresse);
             if (Boucle == 0)
             {
                 Kernel_mbfree (*ptr_Manager);
                 *ptr_Manager = NULL;
                 return RetVal;
             }
             else
             {
                for (Boucle2 = Boucle; Boucle2 < LDT_MANAGER_SIZE-1; Boucle2++)
                { Manager->LDTdesc [Boucle2] = Manager->LDTdesc [Boucle2+1]; }

                Manager->LDTdesc [Boucle2+1].Size    = NULL;
                Manager->LDTdesc [Boucle2+1].Adresse = NULL;
             }

             Manager->LDTdesc [LDT_MANAGER_SIZE-1].Size    = NULL;
             Manager->LDTdesc [LDT_MANAGER_SIZE-1].Adresse = NULL;

             return RetVal;
          }
       }

       *ptr_Manager = Manager;
       Manager      = Manager->Next;
    } while (1);

}

T_LDT * LDT_brealloc (T_LDT * OldLDT, UWORD NewSize)
{
    T_LDT * NewLDT;
    UDWORD Blocs;
    UWORD  Boucle;

    T_LDT_Manager * * ptr_Manager = &LDT_Manager;
    T_LDT_Manager *   Manager     = LDT_Manager;
    Blocs = NewSize * sizeof(T_LDT);

    if (Blocs % MEMORY_BLOC_SIZE == 0)
    { Blocs /= MEMORY_BLOC_SIZE; } else { Blocs = (Blocs / MEMORY_BLOC_SIZE) + 1; }

    do
    {
       if (*ptr_Manager == NULL) { return NULL; }
       for (Boucle = 0; Boucle < LDT_MANAGER_SIZE; Boucle++)
       {
          if (Manager->LDTdesc [Boucle].Adresse == OldLDT)
          {
             NewLDT = Kernel_mbrealloc (OldLDT, Blocs);
             if (NewLDT == NULL) return NULL;
             Manager->LDTdesc [Boucle].Adresse = NewLDT;
             Manager->LDTdesc [Boucle].Size    = NewSize;
             return NewLDT;
          }
       }

       *ptr_Manager = Manager;
       Manager      = Manager->Next;
    } while (1);
}

///////////////////////////////////////////////////////////////////////

T_LDT * LDT_AllocLDT (T_LDT *BaseLDT)
{
    UWORD Boucle;
    UDWORD Boucle2;
    UDWORD Size;
    T_LDT_Manager * Manager = LDT_Manager;

    if (BaseLDT == NULL) return NULL;

    do
    {
       if (Manager == NULL)  { return NULL; }
       for (Boucle = 0; Boucle < LDT_MANAGER_SIZE; Boucle++)
       {
          Size = Manager->LDTdesc [Boucle].Size;
          if (Manager->LDTdesc [Boucle].Adresse == ((void*)BaseLDT) )
          {
            for (Boucle2 = 1; Boucle2 < Size; Boucle2++) // Le 1e LDT reste null
            {
               if ( BaseLDT[Boucle2].UserDefine == 0 )
               {
                  BaseLDT[Boucle2].UserDefine = 1;
                  return BaseLDT+Boucle2;
               };
            }
            return NULL;
          }
       }
       Manager = Manager->Next;
    } while (1);
}

int LDT_FreeLDT (T_LDT *LDT)  // Marque le LDT comme non utilise et le met a NULL
{
    if ( ((UDWORD)LDT & 0x07L) != NULL)  { return -2; }
    if (LDT->UserDefine == 0) { return -1; }
    *((int64*)LDT) = NULL;
    return 0;
}

///////////////////////////////////////////////////////////////////////

int LDT_MapMemory (T_LDT *LDT, void * Linear, UDWORD Size, UBYTE Granularite, UBYTE DPL, UBYTE Type, UBYTE CPUMode)
{
   if ( ((UDWORD)LDT & 0x07L) != NULL)  { return -2; }

   LDT->Adresse_0_15  = (DWORD)Linear & 0x0000FFFFL;
   LDT->Adresse_16_23 = ((DWORD)Linear >> 16)  & 0x000000FFL;
   LDT->Adresse_24_31 = ((DWORD)Linear >> 24) & 0x000000FFL;
   LDT->Limite_0_15   = Size & 0x0000FFFFL;
   LDT->Limite_16_19  = (Size >> 16) & 0x0000000FL;
   LDT->Granularite   = Granularite;
   LDT->Type          = Type;
   LDT->DPL           = DPL;
   LDT->CPUMode       = CPUMode;
   LDT->Present       = 1;
   LDT->UserDefine    = 1;
   return 0;
}

int LDT_load (T_LDT *LDT)
{
    UWORD  Boucle;
    T_LDT_Manager * * ptr_Manager = &LDT_Manager;
    T_LDT_Manager *   Manager     = LDT_Manager;

    do
    {
       if (*ptr_Manager == NULL) { return -1; }
       for (Boucle = 0; Boucle < LDT_MANAGER_SIZE; Boucle++)
       {
          if (Manager->LDTdesc [Boucle].Adresse == LDT)
          {
             asm("lgdt %0\n" : : "m" (Manager->LDTdesc [Boucle]));
             return 0;
          }
       }

       *ptr_Manager = Manager;
       Manager      = Manager->Next;
    } while (1);
}


