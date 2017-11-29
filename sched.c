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
  scheduler->selectorIdle = GDT_TSS_IDLE << 3;
  for(i = 0; i < 16; i++){
    scheduler->presente[i] = 0;
    if(i%2 == 0){
      scheduler->selectors[i] = (GDT_TSS_PIRATA_INICIAL + i) << 3;
    }
  }
}
