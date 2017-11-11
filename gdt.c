/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de la tabla de descriptores globales
*/

#include "gdt.h"




/* Definicion de la GDT */
/* -------------------------------------------------------------------------- */

gdt_entry gdt[GDT_COUNT] = {
  /* Descriptor nulo*/
  /* Offset = 0x00 */
  [GDT_IDX_NULL_DESC] = (gdt_entry) {
    (unsigned short)    0x0000,         /* limit[0:15]  */
    (unsigned short)    0x0000,         /* base[0:15]   */
    (unsigned char)     0x00,           /* base[23:16]  */
    (unsigned char)     0x00,           /* type         */
    (unsigned char)     0x00,           /* s            */
    (unsigned char)     0x00,           /* dpl          */
    (unsigned char)     0x00,           /* p            */
    (unsigned char)     0x00,           /* limit[16:19] */
    (unsigned char)     0x00,           /* avl          */
    (unsigned char)     0x00,           /* l            */
    (unsigned char)     0x00,           /* db           */
    (unsigned char)     0x00,           /* g            */
    (unsigned char)     0x00,           /* base[31:24]  */
  },
  [GDT_IDX_ROOT_CODE] = (gdt_entry) {
    (unsigned short)    0xF3FF,         /* limit[0:15]  */ // 128000
    (unsigned short)    0x0000,         /* base[0:15]   */ // 0
    (unsigned char)     0x00,           /* base[23:16]  */ // 0
    (unsigned char)     0b1000,         /* type         */ // Execute only
    (unsigned char)     0x01,           /* s            */ // No es de sistema (lógica invertida)
    (unsigned char)     0x00,           /* dpl          */ // Nivel 0
    (unsigned char)     0x01,           /* p            */ // ¡Presente!
    (unsigned char)     0x01,           /* limit[16:19] */ // 128000
    (unsigned char)     0x00,           /* avl          */
    (unsigned char)     0x00,           /* l            */ // Para 64 bit noma
    (unsigned char)     0x01,           /* db           */ // 32 bits
    (unsigned char)     0x01,           /* g            */ // De a 4kb
    (unsigned char)     0x00,           /* base[31:24]  */ // 0
  },
  [GDT_IDX_ROOT_DATA] = (gdt_entry) {
    (unsigned short)    0xF3FF,         /* limit[0:15]  */
    (unsigned short)    0x0000,         /* base[0:15]   */
    (unsigned char)     0x00,           /* base[23:16]  */
    (unsigned char)     0b0010,         /* type         */ // Read/Write
    (unsigned char)     0x01,           /* s            */
    (unsigned char)     0x00,           /* dpl          */
    (unsigned char)     0x01,           /* p            */
    (unsigned char)     0x01,           /* limit[16:19] */
    (unsigned char)     0x00,           /* avl          */
    (unsigned char)     0x00,           /* l            */
    (unsigned char)     0x01,           /* db           */
    (unsigned char)     0x01,           /* g            */
    (unsigned char)     0x00,           /* base[31:24]  */
  },
  [GDT_IDX_USER_CODE] = (gdt_entry) {
    (unsigned short)    0xF3FF,         /* limit[0:15]  */
    (unsigned short)    0x0000,         /* base[0:15]   */
    (unsigned char)     0x00,           /* base[23:16]  */
    (unsigned char)     0b1000,         /* type         */
    (unsigned char)     0x01,           /* s            */
    (unsigned char)     0x03,           /* dpl          */
    (unsigned char)     0x01,           /* p            */
    (unsigned char)     0x01,           /* limit[16:19] */
    (unsigned char)     0x00,           /* avl          */
    (unsigned char)     0x00,           /* l            */
    (unsigned char)     0x01,           /* db           */
    (unsigned char)     0x01,           /* g            */
    (unsigned char)     0x00,           /* base[31:24]  */
  },
  [GDT_IDX_USER_DATA] = (gdt_entry) {
    (unsigned short)    0xF3FF,         /* limit[0:15]  */
    (unsigned short)    0x0000,         /* base[0:15]   */
    (unsigned char)     0x00,           /* base[23:16]  */
    (unsigned char)     0b0010,         /* type         */
    (unsigned char)     0x01,           /* s            */
    (unsigned char)     0x03,           /* dpl          */
    (unsigned char)     0x01,           /* p            */
    (unsigned char)     0x01,           /* limit[16:19] */
    (unsigned char)     0x00,           /* avl          */
    (unsigned char)     0x00,           /* l            */
    (unsigned char)     0x01,           /* db           */
    (unsigned char)     0x01,           /* g            */
    (unsigned char)     0x00,           /* base[31:24]  */
  },
  /* Agregamos el descriptor de video */
  [GDT_VIDEO] = (gdt_entry) {
    (unsigned short)    0x1F3F,         /* limit[0:15]  */ // 8000-1 = 80*50*2-1
    (unsigned short)    0x8000,         /* base[0:15]   */ // B8000
    (unsigned char)     0x0B,           /* base[23:16]  */ // B8000
    (unsigned char)     0b0010,         /* type         */ // Read/Write Data
    (unsigned char)     0x01,           /* s            */ // No es de sistema
    (unsigned char)     0x00,           /* dpl          */ // Nivel 0
    (unsigned char)     0x01,           /* p            */ // Presente!
    (unsigned char)     0x00,           /* limit[16:19] */ // 8000
    (unsigned char)     0x00,           /* avl          */
    (unsigned char)     0x00,           /* l            */
    (unsigned char)     0x01,           /* db           */
    (unsigned char)     0x00,           /* g            */
    (unsigned char)     0x00,           /* base[31:24]  */ // B8000
  },
  /* /\* Agregamos el descriptor para las interrupciones *\/ */
  /* [GDT_INT] = (gdt_entry) { */
  /*   (unsigned short)    0xF3FF,         /\* limit[0:15]  *\/ // 128000 */
  /*   (unsigned short)    0x0000,         /\* base[0:15]   *\/ // 0 */
  /*   (unsigned char)     0x00,           /\* base[23:16]  *\/ // 0 */
  /*   (unsigned char)     0b1111,         /\* type         *\/ // Trap Gate */
  /*   (unsigned char)     0x00,           /\* s            *\/ // Es de sistema (lógica invertida) */
  /*   (unsigned char)     0x00,           /\* dpl          *\/ // Nivel 0 */
  /*   (unsigned char)     0x01,           /\* p            *\/ // ¡Presente! */
  /*   (unsigned char)     0x01,           /\* limit[16:19] *\/ // 128000 */
  /*   (unsigned char)     0x00,           /\* avl          *\/ */
  /*   (unsigned char)     0x00,           /\* l            *\/ // Para 64 bit noma */
  /*   (unsigned char)     0x01,           /\* db           *\/ // 32 bits */
  /*   (unsigned char)     0x01,           /\* g            *\/ // De a 4kb */
  /*   (unsigned char)     0x00,           /\* base[31:24]  *\/ // 0 */
  /* }, */
};

gdt_descriptor GDT_DESC = {
  sizeof(gdt) - 1,
  (unsigned int) &gdt
};
