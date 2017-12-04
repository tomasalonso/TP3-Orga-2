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
    unsigned int selectorsPirates[16];

    unsigned int selectorIdle;
} sched_t;

sched_t scheduler;

extern jugador_t jugadorA, jugadorB;

void inicializar_scheduler();

uint sched_proxima_a_ejecutar();

uint sched_hay_proximo_pirata(jugador_t *j);

uint sched_proximo_pirata(jugador_t *j);

uint sched_tick();

#endif	/* !__SCHED_H__ */
