/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
/* Atributos paginas */
/* -------------------------------------------------------------------------- */


/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */

// Ejercicio 4.a
unsigned int proxima_pagina_libre;

// Ejercicio 3.b
void mmu_inicializar_dir_kernel() {
  int i;
  /* Page directory, YEAH (?) */
  pd_entry *pd = (pd_entry *) 0x27000;

  // Seteamos todas las entradas a 0
  for (i = 0; i < 1024; i++) {
    pd[i] = (pd_entry) {
      (unsigned char)   0, // present
      (unsigned char)   1, // read/write
      (unsigned char)   0, // user/supervisor
      (unsigned char)   0, // write-through
      (unsigned char)   0, // cache disabled
      (unsigned char)   0, // accessed
      (unsigned char)   0, // reserved
      (unsigned char)   0, // page size
      (unsigned char)   0, // global page
      (unsigned char)   0, // available
      (unsigned int)    0, // base address
    };
  }


  /* Page table, OH YEAH (?) */
  pt_entry *pt = (pt_entry *) 0x28000;

  // Seteamos las entradas de la primera page table
  // Páginas para el kernel (i < 256)y para el área libre (256 < i < 1024)
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
  pd[0].p = 1;
  pd[0].base = 0b101000;
}

// Ejercicio 4.a
void mmu_inicializar() {
  proxima_pagina_libre = INICIO_PAGINAS_LIBRES; // 0x100000

  mmu_mapear_pagina(0x500000, rcr3(), 0x400000);
}

unsigned int mmu_proxima_pagina_fisica_libre() {
  unsigned int pagina_libre = proxima_pagina_libre;

  proxima_pagina_libre += PAGE_SIZE;

  return pagina_libre;
}

// Ejercicio 4.c
void inicializar_tabla(pt_entry* pt) {
  int i;

  for (i = 0; i < 1024; i++) {
    pt[i] = (pt_entry) {
      (unsigned char)   0, // present
      (unsigned char)   1, // read/write
      (unsigned char)   0, // user/supervisor
      (unsigned char)   0, // write-through
      (unsigned char)   0, // cache disabled
      (unsigned char)   0, // accessed
      (unsigned char)   0, // dirty
      (unsigned char)   0, // page table attribute index
      (unsigned char)   0, // global page
      (unsigned char)   0, // available
      (unsigned int)    0, // base address
    };
  }
}

void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica) {
  // Seteamos los bits restantes que no son de la dirección a 0
  pd_entry *pd = (pd_entry *) (cr3 & 0xFFFFF000);

  // Obtenemos los 10 bits más significativos
  unsigned int pd_index = virtual >> 22;
  // Obtenemos los bits 13 a 22
  // Shifteamos y borramos los bits más altos
  unsigned int pt_index = (virtual >> 12) & 0x3FF;
  // Obtenemos los 12 bits menos significativos
  // unsigned int page_index = virtual & 0xFFF;

  // OJO con esto, chequea que el offset de la página sea igual en ambos
  /* if ((virtual & 0xFFF) != (fisica & 0xFFF)) { */
  /*   __asm __volatile("int \2"); */
  /* } */

  unsigned int pt_base = fisica >> 12;

  pt_entry *pt;
  if (!pd[pd_index].p) {
    // crear page table
    pt = (pt_entry *) mmu_proxima_pagina_fisica_libre();
    inicializar_tabla(pt);
    pd[pd_index].p = 1;
    pd[pd_index].base = (unsigned int) pt >> 12;
  } else {
    pt = (pt_entry *) (pd[pd_index].base << 12);
  }

  pt[pt_index].p = 1;
  pt[pt_index].rw = 1;
  pt[pt_index].us = 1;
  pt[pt_index].wt = 0;
  pt[pt_index].cd = 0;
  pt[pt_index].a = 0;
  pt[pt_index].d = 0;
  pt[pt_index].pi = 0;
  pt[pt_index].gp = 0;
  pt[pt_index].avl = 0;
  pt[pt_index].base = pt_base;

  tlbflush();
}

void mmu_unmapear_pagina(unsigned int virtual, unsigned int cr3) {
}
