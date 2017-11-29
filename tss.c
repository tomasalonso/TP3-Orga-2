/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"

#define base_0_15(dir) ((unsigned int) dir) & 0x0000FFFF
#define base_31_24(dir) (((unsigned int) dir) >> 24) & 0x000000FF
#define base_23_16(dir) (((unsigned int) dir) >> 16) & 0x000000FF

tss tss_inicial;
tss tss_idle;

tss libre;

tss tss_jugadorA[MAX_CANT_PIRATAS_VIVOS];
tss tss_jugadorB[MAX_CANT_PIRATAS_VIVOS];

void tss_inicializar() {

  gdt[GDT_TSS_INICIAL].p = 1;
  gdt[GDT_TSS_INICIAL].base_0_15 = base_0_15(&tss_inicial);
  gdt[GDT_TSS_INICIAL].base_23_16 = base_23_16(&tss_inicial);
  gdt[GDT_TSS_INICIAL].base_31_24 = base_31_24(&tss_inicial);

  tss_inicial = (tss) { 0 };

  gdt[GDT_TSS_IDLE].p = 1;
  // La tss de la tarea idle se ubica a continuación de la tss inicial
  gdt[GDT_TSS_IDLE].base_0_15 = base_0_15(&tss_idle);
  gdt[GDT_TSS_IDLE].base_23_16 = base_23_16(&tss_idle);
  gdt[GDT_TSS_IDLE].base_31_24 = base_31_24(&tss_idle);

  tss_idle = (tss) {
    (unsigned short)  0x00, // ptl
    (unsigned short)  0x00,
    (unsigned int)    0x27000, //pila nivel 0
    (unsigned short)  GDT_IDX_ROOT_CODE << 3, // ss0 = segmento de codigo root
    (unsigned short)  0x00,
    (unsigned int)    0x00, // esp1
    (unsigned short)  0x00, // ss1
    (unsigned short)  0x00,
    (unsigned int)    0x00, // esp2
    (unsigned short)  0x00, // ss2
    (unsigned short)  0x00,
    (unsigned int)    rcr3(), // cr3 = cr3 actual = cr3 kernel
    (unsigned int)    0x00016000, // eip
    (unsigned int)    0x00000202, // eflags
    (unsigned int)    0x00, // eax
    (unsigned int)    0x00, // ecx
    (unsigned int)    0x00, // edx
    (unsigned int)    0x00, // ebx
    (unsigned int)    0x27000, // esp
    (unsigned int)    0x27000, // ebp
    (unsigned int)    0x00, // esi
    (unsigned int)    0x00, //edi
    (unsigned short)  GDT_IDX_ROOT_DATA << 3, // es
    (unsigned short)  0x00,
    (unsigned short)  GDT_IDX_ROOT_CODE << 3, // cs
    (unsigned short)  0x00,
    (unsigned short)  GDT_IDX_ROOT_DATA << 3, // ss
    (unsigned short)  0x00,
    (unsigned short)  GDT_IDX_ROOT_DATA << 3, // ds
    (unsigned short)  0x00,
    (unsigned short)  GDT_IDX_ROOT_DATA << 3, // fs
    (unsigned short)  0x00,
    (unsigned short)  GDT_IDX_ROOT_DATA << 3, // gs
    (unsigned short)  0x00,
    (unsigned short)  0x00, //ldt
    (unsigned short)  0x00,
    (unsigned short)  0x00, // dtrap
    (unsigned short)  0xFFFF, //iomap
  };
}

/* void tss_libre() { */
/*   gdt[GDT_TSS_INICIAL].p = 1; */
/*   gdt[GDT_TSS_INICIAL].base_0_15 = base_0_15(&tss_libre); */
/*   gdt[GDT_TSS_INICIAL].base_23_16 = base_23_16(&tss_libre); */
/*   gdt[GDT_TSS_INICIAL].base_31_24 = base_31_24(&tss_libre); */

/*   unsigned int pila_nivel_0 = mmu_proxima_pagina_fisica_libre(); */
/*   pd_entry* page_directory = mmu_inicializar_dir_pirata(jugadorA); */

/*   tss_idle = (tss) { */
/*     (unsigned short)  0x00, // ptl */
/*     (unsigned short)  0x00, */
/*     (unsigned int)    pila_nivel_0 + 0x1000, // esp0 = pila nivel 0 */
/*     (unsigned short)  GDT_IDX_ROOT_CODE << 3, // ss0 = segmento de codigo root */
/*     (unsigned short)  0x00, */
/*     (unsigned int)    0x00, // esp1 */
/*     (unsigned short)  0x00, // ss1 */
/*     (unsigned short)  0x00, */
/*     (unsigned int)    0x00, // esp2 */
/*     (unsigned short)  0x00, // ss2 */
/*     (unsigned short)  0x00, */
/*     (unsigned int)    page_directory, // cr3 = cr3 actual = cr3 kernel */
/*     (unsigned int)    0x00016000, // eip */
/*     (unsigned int)    0x00000202, // eflags */
/*     (unsigned int)    0x00, // eax */
/*     (unsigned int)    0x00, // ecx */
/*     (unsigned int)    0x00, // edx */
/*     (unsigned int)    0x00, // ebx */
/*     (unsigned int)    0x27000, // esp */
/*     (unsigned int)    0x27000, // ebp */
/*     (unsigned int)    0x00, // esi */
/*     (unsigned int)    0x00, //edi */
/*     (unsigned short)  GDT_IDX_ROOT_DATA << 3, // es */
/*     (unsigned short)  0x00, */
/*     (unsigned short)  GDT_IDX_ROOT_CODE << 3, // cs */
/*     (unsigned short)  0x00, */
/*     (unsigned short)  GDT_IDX_ROOT_DATA << 3, // ss */
/*     (unsigned short)  0x00, */
/*     (unsigned short)  GDT_IDX_ROOT_DATA << 3, // ds */
/*     (unsigned short)  0x00, */
/*     (unsigned short)  GDT_IDX_ROOT_DATA << 3, // fs */
/*     (unsigned short)  0x00, */
/*     (unsigned short)  GDT_IDX_ROOT_DATA << 3, // gs */
/*     (unsigned short)  0x00, */
/*     (unsigned short)  0x00, //ldt */
/*     (unsigned short)  0x00, */
/*     (unsigned short)  0x00, // dtrap */
/*     (unsigned short)  0xFFFF, //iomap */
/*   }; */
/* } */

void inicializar_tss_pirata() {
  int i;
  jugador_t actual;
  tss *arrayActual;

  for(i = 0; i < 16; i++){
    if(i%2 == 0){
      actual = jugadorA;
      arrayActual = tss_jugadorA;
    } else {
      actual = jugadorB;
      arrayActual = tss_jugadorB;
    }
    gdt[GDT_TSS_PIRATA_INICIAL + i].p = 1;
    gdt[GDT_TSS_PIRATA_INICIAL + i].base_0_15 = base_0_15(&arrayActual[i]);
    gdt[GDT_TSS_PIRATA_INICIAL + i].base_23_16 = base_23_16(&arrayActual[i]);
    gdt[GDT_TSS_PIRATA_INICIAL + i].base_31_24 = base_31_24(&arrayActual[i]);

    unsigned int pila_nivel_0 = mmu_proxima_pagina_fisica_libre();
    /* pd_entry* page_directory = mmu_inicializar_dir_pirata(actual); */

    tss_idle = (tss) {
      (unsigned short)  0x00, // ptl
      (unsigned short)  0x00,
      (unsigned int)    pila_nivel_0 + 0x1000, // esp0 = pila nivel 0
      (unsigned short)  GDT_IDX_ROOT_CODE << 3, // ss0 = segmento de codigo root
      (unsigned short)  0x00,
      (unsigned int)    0x00, // esp1
      (unsigned short)  0x00, // ss1
      (unsigned short)  0x00,
      (unsigned int)    0x00, // esp2
      (unsigned short)  0x00, // ss2
      (unsigned short)  0x00,
      (unsigned int)    0,//page_directory, // cr3 = cr3 actual = cr3 kernel
      (unsigned int)    0x00016000, // eip
      (unsigned int)    0x00000202, // eflags
      (unsigned int)    0x00, // eax
      (unsigned int)    0x00, // ecx
      (unsigned int)    0x00, // edx
      (unsigned int)    0x00, // ebx
      (unsigned int)    0x27000, // esp
      (unsigned int)    0x27000, // ebp
      (unsigned int)    0x00, // esi
      (unsigned int)    0x00, //edi
      (unsigned short)  GDT_IDX_USER_DATA << 3, // es
      (unsigned short)  0x00,
      (unsigned short)  GDT_IDX_USER_CODE << 3, // cs
      (unsigned short)  0x00,
      (unsigned short)  GDT_IDX_USER_DATA << 3, // ss
      (unsigned short)  0x00,
      (unsigned short)  GDT_IDX_USER_DATA << 3, // ds
      (unsigned short)  0x00,
      (unsigned short)  GDT_IDX_USER_DATA << 3, // fs
      (unsigned short)  0x00,
      (unsigned short)  GDT_IDX_USER_DATA << 3, // gs
      (unsigned short)  0x00,
      (unsigned short)  0x00, //ldt
      (unsigned short)  0x00,
      (unsigned short)  0x00, // dtrap
      (unsigned short)  0xFFFF, //iomap
    };
  };
}
