/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "game.h"

typedef struct sched_t
{
    unsigned int selectors[16];
    unsigned int presente[16];

    unsigned int selectorIdle;
    unsigned int tareaActual;
} sched_t;

sched_t scheduler;

extern jugador_t jugadorA, jugadorB;

void inicializar_scheduler();

#endif	/* !__SCHED_H__ */
