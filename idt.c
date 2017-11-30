
/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "defines.h"
#include "idt.h"
#include "isr.h"

#include "tss.h"

idt_entry idt[255] = { };

idt_descriptor IDT_DESC = {
    sizeof(idt) - 1,
    (unsigned int) &idt
};

/*
    La siguiente es una macro de EJEMPLO para ayudar a armar entradas de
    interrupciones. Para usar, descomentar y completar CORRECTAMENTE los
    atributos y el registro de segmento. Invocarla desde idt_inicializar() de
    la siguiene manera:

    void idt_inicializar() {
        IDT_ENTRY(0);
        ...
        IDT_ENTRY(19);

        ...
    }
*/

/*
#define IDT_ENTRY(numero)                                             \
  idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF);        \
  idt[numero].segsel = (unsigned short) 0x00;                                                                  \
  idt[numero].attr = (unsigned short) 0x0000;                                                                  \
  idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);
*/

// Ejercicio 2.a
// Pasamos el descriptor de segmento de código
#define IDT_ENTRY(numero, attribute)                                              \
  idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF); \
  idt[numero].segsel = (unsigned short) (GDT_IDX_ROOT_CODE << 3); \
  idt[numero].attr = (unsigned short) attribute;                       \
  idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);

#define TRAP      0b1000111100000000
#define INTERRUPT 0b1000111000000000
#define USER_INTERRUPT 0b1110111000000000

void idt_inicializar() {
  // Ejercicio 2.a
  /* Cargamos las 19 entradas */
  IDT_ENTRY(0, TRAP);
  IDT_ENTRY(1, TRAP);
  IDT_ENTRY(2, TRAP);
  IDT_ENTRY(3, TRAP);
  IDT_ENTRY(4, TRAP);
  IDT_ENTRY(5, TRAP);
  IDT_ENTRY(6, TRAP);
  IDT_ENTRY(7, TRAP);
  IDT_ENTRY(8, TRAP);
  IDT_ENTRY(9, TRAP);
  IDT_ENTRY(10, TRAP);
  IDT_ENTRY(11, TRAP);
  IDT_ENTRY(12, TRAP);
  IDT_ENTRY(13, TRAP);
  IDT_ENTRY(14, TRAP);
  /* IDT_ENTRY(15); // reservada de Intel */
  IDT_ENTRY(16, TRAP);
  IDT_ENTRY(17, TRAP);
  IDT_ENTRY(18, TRAP);
  IDT_ENTRY(19, TRAP);

  // Ejercicio 5.a
  /* Agregamos entrada para interrupción del clock */
  IDT_ENTRY(32, INTERRUPT);

  /* Agregamos entrada para interrupción del teclado */
  IDT_ENTRY(33, INTERRUPT);

  /* Agregamos entrada para interrupción de software 0x46 */
  IDT_ENTRY(70, USER_INTERRUPT);
}
