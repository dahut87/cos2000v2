/***********************************/
/* Librairie Kmemory.h             */
/* 20 Mars 2007                    */
/***********************************/


#ifndef JREKCED_KERNEL_MEMORY
# define JREKCED_KERNEL_MEMORY

# include "type.h"

# define MEMORY_BLOC_SIZE (1024L)       /* Taille des blocs */

                                                // Initialise les gestionnaire de memoire.
void Kernel_InitMemory (void *DataMemory,       // DataMemory pointe vers la zone de memoire disponible.
                       UDWORD TotalMemorySize); // TotalMemorySize est la quantite total de memoire disponible.

void * Kernel_mballoc(UDWORD Size);    // Alloue SIZE blocs de memoire et retourne un pointeur sur la zone.
                                       // Retourne un pointeur nul si l'allocation est impossible.

int    Kernel_mbfree (void *Adresse);  // Libere le bloc memoire alloue a l'adresse ADRESSE.
                                       // Retourne -1 si adresse incorrecte et 0 si tout s'est bien deroule.

void * Kernel_mbrealloc (void *Adresse, UDWORD Size); // Redimensionne un bloc, retourne NULL si le bloc
                                                      // n'est pas redimensionne.
#endif
