/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"
#include "screen.h"

void sched_inicializar() {
  int i;
  int j;

  scheduler.jugadorActual = 0;

  for (i = 0; i < 2; i++) {
    scheduler.slotActual[i] = 7; // último(8) para usar la lógica de slot_libre

    for (j = 0; j < 8; j++) {
      scheduler.slots[i][j] = LIBRE;
    }
  }

  for(i = 0; i < 16; i++) {
    scheduler.selectores[i] = (GDT_TSS_PIRATA_INICIAL + i);
  }

  scheduler.selectores[16] = GDT_TSS_IDLE;
}

uint sched_proxima_a_ejecutar() {
  uint jActivo = scheduler.jugadorActual;
  uint jInactivo = (jActivo == 0) ? 1 : 0;
  uint proximo = 16; // IDLE por defecto

  if (sched_hay_slot_a_ejecutar(jInactivo)) {
    scheduler.jugadorActual = jInactivo;

    proximo = sched_proximo_slot_a_ejecutar(jInactivo);
  } else if (sched_hay_slot_a_ejecutar(jActivo)){
    proximo = sched_proximo_slot_a_ejecutar(jActivo);
  }

  return proximo;
}

uint sched_hay_slot_a_ejecutar(uint j) {
  uint i;
  for (i = 0; i < 8; i++) {
    if (scheduler.slots[j][i] == EJECUCION) {
      return 1;
    }
  }

  return 0;
}

uint sched_proximo_slot_a_ejecutar(uint j) {
  uint proximo;
  uint slotIni = scheduler.slotActual[j]+1;

  uint i;
  for (i = slotIni; i < slotIni+8; i++) {
    if (scheduler.slots[j][i%8] == EJECUCION) {
      proximo = i%8;
      scheduler.slotActual[j] = proximo;
      break;
    }
  }

  return proximo;
}

uint sched_hay_slot_libre(uint j) {
  uint i;
  for (i = 0; i < 8; i++) {
    if (scheduler.slots[j][i] == LIBRE) {
      return 1;
    }
  }

  return 0;
}

uint sched_proximo_slot_libre(uint j) {
  uint proximo;
  uint slotIni = scheduler.slotActual[j]+1;

  uint i;
  for (i = slotIni; i < slotIni+8; i++) {
    if (scheduler.slots[j][i%8] == LIBRE) {
      proximo = i%8;
      scheduler.slots[j][i%8] = EJECUCION;
      break;
    }
  }

  return proximo;
}

uint sched_tick() {
  game_tick(0);

  uint proxTarea = sched_proxima_a_ejecutar();

  print_hex(scheduler.selectores[proxTarea], 20, 0, 0, 0xF0);

  return scheduler.selectores[proxTarea];
}

pirata_t * sched_pirata_actual() {
  jugador_t *j = (scheduler.jugadorActual == 0) ? &jugadorA : & jugadorB;

  return &(j->piratas[scheduler.slotActual[scheduler.jugadorActual]]);
}

jugador_t * sched_jugador_actual() {
  jugador_t *j = (scheduler.jugadorActual == 0) ? &jugadorA : & jugadorB;

  return j;
}
