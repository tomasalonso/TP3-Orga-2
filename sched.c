/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"

void inicializar_scheduler(){
  int i;
  scheduler.selectorIdle = GDT_TSS_IDLE;

  for(i = 0; i < 16; i++){
    if(i%2 == 0){
      scheduler.selectorsPirates[i%8] = (GDT_TSS_PIRATA_INICIAL + i);
    } else {
      scheduler.selectorsPirates[i%8 + 8] = (GDT_TSS_PIRATA_INICIAL + i);
    }
  }
}

uint sched_proxima_a_ejecutar() {
  if (!jugadorA.activo && !jugadorB.activo) {
    return 16; // tarea IDLE
  }

  jugador_t *jActivo = (jugadorA.activo) ? &jugadorA : &jugadorB;
  jugador_t *jInactivo = (jugadorA.activo) ? &jugadorB : &jugadorA;
  uint proximo;

  if (sched_hay_proximo_pirata(jInactivo)) {
    jActivo->activo = 0;
    jInactivo->activo = 1;

    proximo = sched_proximo_pirata(jInactivo);
  } else {
    proximo = sched_proximo_pirata(jActivo);
  }

  if (jugadorB.activo)
    proximo += 8;

  return proximo;
}

uint sched_hay_proximo_pirata(jugador_t *j) {
  uint i;
  for (i = j->pirataActual+1; i < j->pirataActual+8+1; i++) {
    // encontrar al primero que sea en ejecución
    if (j->piratas[i % 8].enEjecucion == 1) {
      return 1;
    }
  }

  return 0;
}

uint sched_proximo_pirata(jugador_t *j) {
  uint proximo = 0;

  uint i;
  for (i = j->pirataActual+1; i < j->pirataActual+8+1; i++) {
    // encontrar al primero que sea en ejecución
    if (j->piratas[i % 8].enEjecucion == 1) {
      proximo = i%8; break;
    }
  }

  return proximo;
}

uint sched_tick() {
  game_tick(0);

  uint proxTarea = sched_proxima_a_ejecutar();

  if (proxTarea == 16) {
    return scheduler.selectorIdle;
  }

  uint selector = scheduler.selectorsPirates[proxTarea];

  return scheduler.selectorsPirates[selector];
}
