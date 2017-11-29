/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include "game.h"

/* Direcciones físicas de códigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones están los códigos de todas las tareas. De aquí se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones físicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */

// Ejercicio 4.a
unsigned int proxima_pagina_libre;

// Ejercicio 3.b
void mmu_inicializar_dir_kernel() {
  int i;
  /* Page directory, YEAH (?) */
  pd_entry *pd = (pd_entry *) 0x27000;

  // Toda la page directory a 0
  inicializar_page_struct((unsigned int *) pd);

  /* Page table, OH YEAH (?) */
  pt_entry *pt = (pt_entry *) 0x28000;

  // Seteamos las entradas de la primera page table
  // Páginas para el kernel (i < 256)y para el área libre (256 < i < 1024)
  // identity mapping
  for (i = 0; i < 1024; i++) {
    pt[i] = (pt_entry) {
      (unsigned char)   1, // present
      (unsigned char)   1, // read/write
      (unsigned char)   0, // user/supervisor
      (unsigned char)   0, // write-through
      (unsigned char)   0, // cache disabled
      (unsigned char)   1, // accessed
      (unsigned char)   0, // dirty
      (unsigned char)   0, // page table attribute index
      (unsigned char)   0, // global page
      (unsigned char)   0, // available
      (unsigned int)    i, // base address
    };
  }

  // Iniciamos la primera entrada
  pd[0].rw = 1;
  pd[0].base = 0x28;
  pd[0].p = 1;
}

// Ejercicio 4.a
void mmu_inicializar() {
  proxima_pagina_libre = INICIO_PAGINAS_LIBRES; // 0x100000

  /* prueba de mapear y desmapear pagina */
  //mmu_mapear_pagina(0x400000, rcr3(), 0x500000, RW);
  // breakpoint();
  //mmu_unmapear_pagina(0x400000, rcr3());
}

unsigned int mmu_proxima_pagina_fisica_libre() {
  unsigned int pagina_libre = proxima_pagina_libre;

  proxima_pagina_libre += PAGE_SIZE;

  return pagina_libre;
}

// Ejercicio 4.b
// Tamaño pos_piola = DC0 (tamaño del mapa)
pd_entry* mmu_inicializar_dir_pirata(pirata_t *p) {
  // Inicializar directorio de páginas
  pd_entry* pd_pirata = (pd_entry*) mmu_proxima_pagina_fisica_libre();

  // Seteamos todas las entradas a 0
  inicializar_page_struct((unsigned int*) pd_pirata);

  // Iniciamos la primera entrada
  // identity mapping del kernel
  pd_pirata[0].base = 0x28; // pt compartida con el kernel
  pd_pirata[0].p = 1;


  // Obtenemos dirección del puerto donde comienza la tarea
  unsigned int codigo;
  codigo = game_lineal2physical(game_xy2lineal(p->jugador->puertoX, p->jugador->puertoY));

  // Mapea la dirección del puerto en la 0x400000
  // para copiar el código
  mmu_mapear_pagina(0x400000, rcr3(), codigo, RW);
  unsigned int *page_task; // obtiene la dirección del código de la tarea
  if (p->tipo == EXPLORADOR) {
    page_task = (unsigned int*) p->jugador->codExplorador;
  } else {
    // (tipo == MINERO)
    page_task = (unsigned int*) p->jugador->codMinero;
  }
  // Copiamos de la 10000-10ffff o 12000-12ffff
  // (según el jugador) a la 4000000-4000fff
  copiarPagina((unsigned int*) 0x400000, page_task);
  // Desmapeamos la dirección del mapa
  mmu_unmapear_pagina(0x400000, rcr3());


  // Mapeamos la dirección física del código de la tarea
  // en la dirección virtual 0x400000 como lectura/escritura
  mmu_mapear_pagina(0x400000, (unsigned int) pd_pirata, codigo, RW);


  // Mapear posiciones ya descubiertas
  // 0x800000 virtual
  // asigna las mismas page table del mapa para todas las tareas
  int i;
  for (i = 0; i < 4; i++) {
    pd_pirata[i+4] = (pd_entry) {
      (unsigned char)  1,  // present
      (unsigned char)  1,  // read/write
      (unsigned char)  0,  // user/supervisor
      (unsigned char)  0,  // write-through
      (unsigned char)  0,  // cache disabled
      (unsigned char)  0,  // accessed
      (unsigned char)  0,  // reserved
      (unsigned char)  0,  // page size
      (unsigned char)  0,  // global page
      (unsigned char)  0,  // available
      (unsigned int)   p->jugador->mapa[i], // base address
    };
  }

  return pd_pirata;
}

// Ejercicio 4.c
void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica, unsigned int rw) {
  // Seteamos los bits restantes que no son de la dirección a 0
  pd_entry *pd = (pd_entry *) (cr3 & 0xFFFFF000);

  // Obtenemos los 10 bits más significativos
  unsigned int pd_index = PDE_INDEX(virtual);
  // Obtenemos los bits 13 a 22
  // Shifteamos y borramos los bits más altos
  unsigned int pt_index = PTE_INDEX(virtual);
  // Obtenemos los 12 bits menos significativos
  // unsigned int page_index = virtual & 0xFFF;

  // CUIDADO con esto, chequea que el offset de la página sea igual en ambos
  /* if ((virtual & 0xFFF) != (fisica & 0xFFF)) { */
  /*   __asm __volatile("int \2"); */
  /* } */

  unsigned int pt_base = fisica >> 12;

  pt_entry *pt;
  if (!pd[pd_index].p) {
    // crear page table
    pt = (pt_entry *) mmu_proxima_pagina_fisica_libre();
    inicializar_page_struct((unsigned int *) pt);
    pd[pd_index].p = 1;
    pd[pd_index].rw = 1;
    pd[pd_index].base = (unsigned int) pt >> 12;
  } else {
    pt = (pt_entry *) (pd[pd_index].base << 12);
  }

  pt[pt_index].p = 1;
  pt[pt_index].rw = rw;
  pt[pt_index].us = 1;
  pt[pt_index].wt = 0;
  pt[pt_index].cd = 0;
  pt[pt_index].a = 0;
  pt[pt_index].d = 0;
  pt[pt_index].pi = 0;
  pt[pt_index].gp = 0;
  pt[pt_index].avl = 0;
  pt[pt_index].base = pt_base;

  // Se reescribe cr3 y se refrescan las tlb
  tlbflush();
}

void mmu_unmapear_pagina(unsigned int virtual, unsigned int cr3) {
  // Seteamos los bits restantes que no son de la dirección a 0
  pd_entry *pd = (pd_entry *) (cr3 & 0xFFFFF000);

  // Obtenemos los 10 bits más significativos
  unsigned int pd_index = PDE_INDEX(virtual);
  // Obtenemos los bits 13 a 22
  // Shifteamos y borramos los bits más altos
  unsigned int pt_index = PTE_INDEX(virtual);
  // Obtenemos los 12 bits menos significativos
  // unsigned int page_index = virtual & 0xFFF;

  pt_entry *pt;
  if (!pd[pd_index].p) {
    // la tabla no existe, nada que hacer
    // preguntar
    return;
  }
  pt = (pt_entry *) (pd[pd_index].base << 12);
  pt[pt_index].p = 0;

  // Se reescribe cr3 y se refrescan las tlb
  tlbflush();
}

void inicializar_page_struct(unsigned int* page_struct) {
  int i;

  for (i = 0; i < 1024; i++) {
    page_struct[i] = 0;
  }
}

void copiarPagina(unsigned int *page_dest, unsigned int *page_orig) {
  int i;

  for (i = 0; i < 0x1000; i+= 4) {
    // Copiamos de a 4 bytes
    *(page_dest) = *(page_orig);
    page_dest++;
    page_orig++;
  }
}
