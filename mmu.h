/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "defines.h"
#include "game.h"

#define CODIGO_BASE       0X400000

#define MAPA_BASE_FISICA  0x500000
#define MAPA_BASE_VIRTUAL 0x800000

#define INICIO_PAGINAS_LIBRES 0x100000

// Ejercicio 4.c
// Obtenemos los 10 bits más significativos
#define PDE_INDEX(virtual) virtual >> 22
// Obtenemos los bits 13 a 22
// Shifteamos y borramos los bits más altos
#define PTE_INDEX(virtual) (virtual >> 12) & 0x3FF


// Parámetro para mapear pagina
#define RO       0
#define RW       1

/* Estructuras paginación */
/* -------------------------------------------------------------------------- */
// Entrada de page directory
typedef struct str_pd_entry {
  unsigned char   p:1;    // present
  unsigned char   rw:1;   // read/write
  unsigned char   us:1;   // user/supervisor
  unsigned char   wt:1;   // write-through
  unsigned char   cd:1;   // cache disabled
  unsigned char   a:1;    // accessed
  unsigned char   r:1;    // reserved
  unsigned char   ps:1;   // page size
  unsigned char   gp:1;   // global page
  unsigned char   avl:3;  // available
  unsigned int   base:20; // base address
} __attribute__((__packed__, aligned (4))) pd_entry;

// Entrada de page table
typedef struct str_pt_entry {
  unsigned char   p:1;    // present
  unsigned char   rw:1;   // read/write
  unsigned char   us:1;   // user/supervisor
  unsigned char   wt:1;   // write-through
  unsigned char   cd:1;   // cache disabled
  unsigned char   a:1;    // accessed
  unsigned char   d:1;    // dirty
  unsigned char   pi:1;   // page table attribute index
  unsigned char   gp:1;   // global page
  unsigned char   avl:3;  // available
  unsigned int   base:20; // base address
} __attribute__((__packed__, aligned (4))) pt_entry;


// Ejercicio 3.b
void mmu_inicializar_dir_kernel();

void mmu_inicializar();

// Ejercicio 4.a
unsigned int mmu_proxima_pagina_fisica_libre();

// Ejercicio 4.b
pd_entry* mmu_inicializar_dir_pirata(pirata_t *p);

// Ejercicio 4.c
// lo hace con permisos de lectura/escritura
void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica, unsigned int rw);

void mmu_unmapear_pagina(unsigned int virtual, unsigned int cr3);

// Dada una dirección de una page table o page directory,
// inicializa con p=0 las entradas
void inicializar_page_struct(unsigned int* page_struct);

// Copia el contenido de la página page_orig en page_dest
void copiarPagina(unsigned int *page_dest, unsigned int *page_orig);

#endif	/* !__MMU_H__ */
