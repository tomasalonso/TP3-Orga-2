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

// Ejercicio 4.c


#endif	/* !__MMU_H__ */
