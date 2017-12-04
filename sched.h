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
  unsigned int selectores[17];

  uint jugadorActual;

  uint slotActual[2];

  int slots[2][16];
} sched_t;

sched_t scheduler;

extern jugador_t jugadorA, jugadorB;

void sched_inicializar();

uint sched_proxima_a_ejecutar();

uint sched_hay_slot_a_ejecutar(uint j);

uint sched_proximo_slot_a_ejecutar(uint j);

uint sched_tick();

uint sched_hay_slot_libre(uint j);

uint sched_proximo_slot_libre(uint j);

pirata_t * sched_pirata_actual();

jugador_t * sched_jugador_actual();

#endif	/* !__SCHED_H__ */
