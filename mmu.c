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

void mmu_inicializar_dir_kernel() {
  int i;

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

  // Seteamos las entradas de la primera page table
  // Páginas para el kernel
  for (i = 0; i < 256; i++) {
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
  // Páginas para el área libre
  for (i = 256; i < 1024; i++) {
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
