/***********************************/
/* Librairie Kmemory.c             */
/* 20 Mars 2007                    */
/***********************************/

#include <type.h>
#include <Kmemory.h>

/* Structure table d'allocation memoire */
typedef struct
{
    UBYTE Used : 1;   // = 0 -> La zone memoire est libre.
                      // = 1 -> La zone memoire est utilisee.
    UBYTE Expand : 1; // = 0 -> Fin du bloc.
                      // = 1 -> Le bloc se prolonge sur le bloc suivant.
    UBYTE UnUsed : 6; // Zone libre.
} T_KernelMemoryMap;


T_KernelMemoryMap * BlocAllocate = NULL;  // Table d'allocation des blocs memoire.
void  *             BaseDynAlloc = NULL;  // Pointeur sur la zone memoire allouable.
UDWORD              MaxMemoryBlocs;


// Initialisation du gestionnaire de memoire.
void Kernel_InitMemory (void *DataMemory, UDWORD TotalMemorySize)
{
    UDWORD Boucle;
    UDWORD TableBlocs;
    UDWORD TotalBlocs;

    // Alignement des bloc de memoire a un multiple de MEMORY_BLOC_SIZE
    if ( (UDWORD)DataMemory % MEMORY_BLOC_SIZE != 0)
    {  DataMemory = ((void*) ((((UDWORD)DataMemory) / MEMORY_BLOC_SIZE) * MEMORY_BLOC_SIZE) + MEMORY_BLOC_SIZE );  };

    TotalBlocs = (((void*)TotalMemorySize) - DataMemory) / MEMORY_BLOC_SIZE;
    TableBlocs = TotalBlocs / MEMORY_BLOC_SIZE;
    MaxMemoryBlocs = TotalBlocs - TableBlocs;
    BlocAllocate = (T_KernelMemoryMap *)DataMemory;
    BaseDynAlloc = DataMemory + (TableBlocs * MEMORY_BLOC_SIZE);
    
    for (Boucle = 0; Boucle < MaxMemoryBlocs; Boucle++)
    { 
        BlocAllocate [Boucle].Used   = 0;
        BlocAllocate [Boucle].Expand = 0;
    }
}


// Alloue SIZE blocs de memoire
void * Kernel_mballoc(UDWORD Size)
{
   UDWORD Counter = 0;
   UDWORD Base    = 0;
   UDWORD Boucle, Boucle2;

   if (Size == 0) return NULL;

   for (Boucle = 0; Boucle < MaxMemoryBlocs; Boucle++)
   {
      if (BlocAllocate[Boucle].Used == 0) { Counter++; }
      else { Counter = 0; Base = Boucle + 1; }

      if (Counter >= Size)
      {
         if (Base+Size > MaxMemoryBlocs) return NULL;
         for (Boucle2=Base; Boucle2 < (Base+Size); Boucle2++)
         {
            BlocAllocate[Boucle2].Used   = 1;
            if (Boucle2+1 < (Base+Size) ) { BlocAllocate[Boucle2].Expand = 1; }
            else { BlocAllocate[Boucle2].Expand = 0; }
        }
        return ((void*) (BaseDynAlloc + (Base * MEMORY_BLOC_SIZE) ));
      }
   }
   return NULL;
}

// Libere un bloc memoire
int Kernel_mbfree (void *Adresse)
{
   UDWORD Bloc;

   if (((Adresse - BaseDynAlloc) % MEMORY_BLOC_SIZE) != 0) return -1;
   if ((((UDWORD)Adresse) % MEMORY_BLOC_SIZE) != 0) return -1;

   Bloc = (Adresse - BaseDynAlloc) / MEMORY_BLOC_SIZE;

   for (;  BlocAllocate[Bloc].Expand == 1; Bloc++)
   {
       BlocAllocate[Bloc].Used   = 0;
       BlocAllocate[Bloc].Expand = 0;
   }

   BlocAllocate[Bloc].Used   = 0;
   BlocAllocate[Bloc].Expand = 0;

   return 0;
};

void * Kernel_mbrealloc (void *Adresse, UDWORD Size)
{
   UDWORD Bloc, Boucle, OldSize, CopySize;
   void * NewAdresse;

   if (((Adresse - BaseDynAlloc) % MEMORY_BLOC_SIZE) != 0) return NULL;
   Bloc = (Adresse - BaseDynAlloc) / MEMORY_BLOC_SIZE;
   if (BlocAllocate[Bloc].Used == 0) return NULL;

   for (Boucle = Bloc; BlocAllocate[Boucle].Expand == 1; Boucle++) 
   { if (Boucle >= MaxMemoryBlocs) break; };
   
   OldSize = Boucle - Bloc + 1;

   // Si pas de changement de taille de bloc, ...
   if ( Size == OldSize) return Adresse;

   // Si on reduit le nombre de bloc, ...
   if ( Size < OldSize)
   {
      for (Boucle = (Bloc + Size); Boucle < (Bloc + OldSize) ; Boucle++)
      { 
         BlocAllocate[Boucle].Used   = 0;
         BlocAllocate[Boucle].Expand = 0;
      }
      BlocAllocate[Size-1].Expand = 0;
      return Adresse;
   }
   
   // Si on augmente le nombre de bloc
   for (Boucle = (Bloc + OldSize); Boucle < (Bloc + Size) ; Boucle++)
   {
       // Si le bloc ne peut etre simplement agrandit, ...
       if ( BlocAllocate[Boucle].Used == 1 )
       {
           NewAdresse = Kernel_mballoc (Size);
           if (NewAdresse == NULL) return NULL;
           CopySize = (OldSize * MEMORY_BLOC_SIZE) / sizeof (int64);
           for (Boucle = 0; Boucle < CopySize; Boucle++)
           { ((int64*) NewAdresse)[Boucle] = ((int64*) Adresse)[Boucle]; };
           Kernel_mbfree (Adresse);
           return NewAdresse;
       }
   }
   
   // Le bloc est simplement agrandit
   for (Boucle = (Bloc + OldSize - 1); Boucle < (Bloc + Size) ; Boucle++)
   {
         BlocAllocate[Boucle].Used   = 1;
         BlocAllocate[Boucle].Expand = 1;
   }
   BlocAllocate[Size-1].Expand = 0;
   return Adresse;
}

#undef MAX_MEMORY_BLOCS
