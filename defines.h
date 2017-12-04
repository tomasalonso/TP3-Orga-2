/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Tipos basicos */
/* -------------------------------------------------------------------------- */
#define NULL                    0
#define TRUE                    0x00000001
#define FALSE                   0x00000000
#define ERROR                   1

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

/* Constantes basicas */
/* -------------------------------------------------------------------------- */
#define PAGE_SIZE               0x00001000
#define TASK_SIZE               4096

#define BOOTSECTOR              0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
#define KERNEL                  0x00001200 /* direccion fisica de comienzo del kernel */


/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_COUNT 30

#define GDT_IDX_NULL_DESC 0
#define GDT_IDX_ROOT_CODE 8
#define GDT_IDX_ROOT_DATA 9
#define GDT_IDX_USER_CODE 10
#define GDT_IDX_USER_DATA 11

/* Las agregamos losotros */
// Ejercicio 1.c
#define GDT_VIDEO 12
// Intento para el 2 (no se usa al final)
#define GDT_INT 13 // NO SE USA

// Ejercicio 6.a
#define GDT_TSS_INICIAL 12
#define GDT_TSS_IDLE 13

//Ejercicio 7.a
#define GDT_TSS_PIRATA_INICIAL 14
#define GDT_TSS_A1 14
#define GDT_TSS_A2 15
#define GDT_TSS_A3 16
#define GDT_TSS_A4 17
#define GDT_TSS_A5 18
#define GDT_TSS_A6 19
#define GDT_TSS_A7 20
#define GDT_TSS_A8 21
#define GDT_TSS_B1 22
#define GDT_TSS_B2 23
#define GDT_TSS_B3 24
#define GDT_TSS_B4 25
#define GDT_TSS_B5 26
#define GDT_TSS_B6 27
#define GDT_TSS_B7 28
#define GDT_TSS_B8 29
/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC           (GDT_IDX_NULL_DESC      << 3)

/* Selectores de segmentos */
/* -------------------------------------------------------------------------- */

void* error();

#define ASSERT(x) while(!(x)) {};


#endif  /* !__DEFINES_H__ */
