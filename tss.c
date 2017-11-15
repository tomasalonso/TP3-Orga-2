/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"

#define base_0_15(dir) dir & 0x0000FFFF
#define base_31_24(dir) (dir >>24) & 0x000000FF
#define base_23_16(dir) (dir >> 16) & 0x000000FF

tss tss_inicial;
tss tss_idle;

tss tss_jugadorA[MAX_CANT_PIRATAS_VIVOS];
tss tss_jugadorB[MAX_CANT_PIRATAS_VIVOS];

unsigned int pagina_tss;
void tss_inicializar() {

  pagina_tss = mmu_proxima_pagina_fisica_libre();

  gdt[GDT_TSS_INICIAL].p = 1;
  gdt[GDT_TSS_INICIAL].base_0_15 = base_0_15(pagina_tss);
  gdt[GDT_TSS_INICIAL].base_23_16 = base_23_16(pagina_tss);
  gdt[GDT_TSS_INICIAL].base_31_24 = base_31_24(pagina_tss);

  tss *tss_init = (tss*) pagina_tss;
  *tss_init = (tss) { 0 }; // 38
  /*
  (tss) {
      unsigned short)  0x00, // ptl
      unsigned short  0x00,
      unsigned int    0x27000, //pila nivel 0
      unsigned short  0x09 << 3, // segmento de codigo root
      unsigned short  0x00,
      unsigned int    0x00, // esp1
      unsigned short  0x00, // ss1
      unsigned short  0x00,
      unsigned int    0x00, // esp2
      unsigned short  0x00, // ss2
      unsigned short  0x00,
      unsigned int    rcr3(), // cr3 = cr3 actual = cr3 kernel
      unsigned int    0x00,
      unsigned int    eflags;
      unsigned int    eax;
      unsigned int    ecx;
      unsigned int    edx;
      unsigned int    ebx;
      unsigned int    esp;
      unsigned int    ebp;
      unsigned int    esi;
      unsigned int    edi;
      unsigned short  es;
      unsigned short  unused4;
      unsigned short  cs;
      unsigned short  unused5;
      unsigned short  ss;
      unsigned short  unused6;
      unsigned short  ds;
      unsigned short  unused7;
      unsigned short  fs;
      unsigned short  unused8;
      unsigned short  gs;
      unsigned short  unused9;
      unsigned short  ldt;
      unsigned short  unused10;
      unsigned short  dtrap;
      unsigned short  iomap;
  };
  */

  gdt[GDT_TSS_IDLE].p = 1;
  // La tss de la tarea idle se ubica a continuación de la tss inicial
  gdt[GDT_TSS_IDLE].base_0_15 = base_0_15(pagina_tss + 0x67);
  gdt[GDT_TSS_IDLE].base_23_16 = base_23_16(pagina_tss + 0x67);
  gdt[GDT_TSS_IDLE].base_31_24 = base_31_24(pagina_tss + 0x67);

  tss *tarea_idle = (tss*) pagina_tss + 0x67;

  *tarea_idle = (tss) {
        unsigned short)  0x00, // ptl
        unsigned short  0x00,
        unsigned int    0x27000, //pila nivel 0
        unsigned short  0x09 << 3, // ss0 = segmento de codigo root
        unsigned short  0x00,
        unsigned int    0x00, // esp1
        unsigned short  0x00, // ss1
        unsigned short  0x00,
        unsigned int    0x00, // esp2
        unsigned short  0x00, // ss2
        unsigned short  0x00,
        unsigned int    rcr3(), // cr3 = cr3 actual = cr3 kernel
        unsigned int    0x00016000, // eip
        unsigned int    eflags, // eflags
        unsigned int    0x00, // eax
        unsigned int    0x00, // ecx
        unsigned int    0x00, // edx
        unsigned int    0x00, // ebx
        unsigned int    , // esp
        unsigned int    , // ebp
        unsigned int    0x00, // esi
        unsigned int    0x00, //edi
        unsigned short  es;
        unsigned short  0x00,
        unsigned short  cs;
        unsigned short  0x00,
        unsigned short  ss;
        unsigned short  0x00,
        unsigned short  ds;
        unsigned short  0x00,
        unsigned short  fs;
        unsigned short  0x00,
        unsigned short  gs;
        unsigned short  0x00,
        unsigned short  0x00, //ldt
        unsigned short  0x00,
        unsigned short  0x00, // dtrap
        unsigned short  , //iomap
    };
}
