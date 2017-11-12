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
}

unsigned int mmu_proxima_pagina_fisica_libre() {
  unsigned int pagina_libre = proxima_pagina_libre;
  proxima_pagina_libre += PAGE_SIZE;
  return pagina_libre;
}
