/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"

#include <stdarg.h>


#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      1
#define POS_INIT_B_X         MAPA_ANCHO - 2
#define POS_INIT_B_Y          MAPA_ALTO - 2

#define CANT_POSICIONES_VISTAS            9
#define MAX_SIN_CAMBIOS                 999

#define BOTINES_CANTIDAD 8

// Para syscall 0x46
#define MOVERSE 0x1
#define CAVAR 0x2
#define POSICION 0x3

uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {30,  3, 50}, {31, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {49,  3, 50}, {48, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };

jugador_t jugadorA;
jugador_t jugadorB;


void* error() {
	__asm__ ("int3");
	return 0;
}

uint game_xy2lineal (uint x, uint y) {
	return (y * MAPA_ANCHO + x);
}

uint game_dir2xy(direccion dir, int *x, int *y) {
	switch (dir)
    {
		case IZQ: *x = -1; *y =  0; break;
		case DER: *x =  1; *y =  0; break;
		case ABA: *x =  0; *y =  1; break;
		case ARR: *x =  0; *y = -1; break;
    default: return -1;
    }

	return 0;
}

uint game_posicion_valida(int x, int y) {
	return (x >= 0 && y >= 0 && x < MAPA_ANCHO && y < MAPA_ALTO);
}

// dada una posicion (x,y) guarda las posiciones de alrededor en dos arreglos, uno para las x y otro para las y
void game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y) {
	int next = 0;
	int i, j;
	for (i = -1; i <= 1; i++)
    {
      for (j = -1; j <= 1; j++)
        {
          vistas_x[next] = x + j;
          vistas_y[next] = y + i;
          next++;
        }
    }
}

pirata_t* id_pirata2pirata(uint id_pirata) {
  // ~ completar ~
	return NULL;
}

uint game_valor_tesoro(uint x, uint y) {
	int i;
	for (i = 0; i < BOTINES_CANTIDAD; i++)
	{
		if (botines[i][0] == x && botines[i][1] == y)
			return botines[i][2];
	}
	return 0;
}

void game_inicializar() {
  /* Inicializar jugador A */
  game_jugador_inicializar(&jugadorA);
  /* Inicializar jugador B */
  game_jugador_inicializar(&jugadorB);
  game_jugador_lanzar_pirata(&jugadorA, EXPLORADOR);
}

void game_jugador_inicializar(jugador_t *j) {
	static int index = 0;

	j->index = index++;
  game_pirata_inicializar(j);
  j->activo = 0;
  j->pirataActual = 0;
  j->monedas = 0;

  if (j->index == JUGADOR_A) {
    // JUGADOR_A
    j->codigo[EXPLORADOR] = TASK_AE;
    j->codigo[MINERO] = TASK_AM;
    j->puertoX = POS_INIT_A_X;
    j->puertoY = POS_INIT_A_Y;
  } else {
    // JUGADOR_B
    j->codigo[EXPLORADOR] = TASK_BE;
    j->codigo[MINERO] = TASK_BM;
    j->puertoX = POS_INIT_B_X;
    j->puertoY = POS_INIT_B_Y;
  }

  game_jugador_inicializar_mapa(j);
  print_hex(jugadorA.puertoX, 20, 0, 0, 0x0F);
  print_hex(jugadorA.puertoY, 20, 0, 1, 0x0F);
}

void game_jugador_inicializar_mapa(jugador_t *j) {
  uint i;
  // Pedimos 4 páginas para las page table del mapa,
  // que compartirán todas las tareas
  for (i = 0; i < 4; i++) {
    j->mapa[i] = mmu_proxima_pagina_fisica_libre();

    inicializar_page_struct((unsigned int*) j->mapa[i]);
  }
}

void game_pirata_inicializar(jugador_t *j) {
  int i;

  for (i = 0; i < 8; i++) {
    j->piratas[i].index = i;
    j->piratas[i].jugador = j;
    j->piratas[i].enEjecucion = 0;
  }
}

// Ejercicio 5.b
void game_tick(uint id_pirata) {
  screen_actualizar_reloj_global();
}

void game_pirata_relanzar(pirata_t *pirata, jugador_t *j, uint tipo) {
}

pirata_t* game_jugador_erigir_pirata(jugador_t *j, uint tipo) {
    // ~ completar ~

	return NULL;
}

void game_jugador_lanzar_pirata(jugador_t *j, uint tipo) {
  int slot = -1;

  int i;
  for (i = 0; i < 8; i++) {
    if (!j->piratas[i].enEjecucion) {
      slot = i; break;
    }
  }

  pirata_t *pirata = &j->piratas[slot];

  pirata->tipo = tipo;
  pirata->posicionX = j->puertoX;
  pirata->posicionY = j->puertoY;
  pirata->enEjecucion = 1;

  inicializar_tss_pirata(pirata);
}

void game_explorar_posicion(pirata_t *pirata, int x, int y) {
  // mapea el mapa, cuak
  int v_x[9];
  int v_y[9];

  game_calcular_posiciones_vistas(v_x, v_y, x, y);
  // mapeamos las posiciones del mapa nuevas
  int i;
  for (i = 0; i < 9; i++) {
    game_pirata_habilitar_posicion(v_x[i], v_y[i]);
  }

  // copia el codigo en su nueva posicion
  game_actualizar_codigo(pirata->posicionX, pirata->posicionY, x, y);

  // actualizamos la posición del pirata
  pirata->posicionX = x;
  pirata->posicionY = y;
}

void game_pirata_habilitar_posicion(int x, int y) {
  uint posLineal = game_xy2lineal(x, y);
  // convertimos a posición en memoria virtual y física
  uint virtual = game_lineal2virtual(posLineal);
  uint fisica = game_lineal2physical(posLineal);

  // mapeamos la posición del mapa
  // (tranquilamente podría ya haber sido mapeada
  //  por otro explorador)
  mmu_mapear_pagina(virtual, rcr3(), fisica, RO);
}

uint game_syscall_pirata_mover(jugador_t *j, direccion dir) {
    pirata_t *pirata = &j->piratas[j->pirataActual];

    int mov_x, mov_y;

    uint dirValida = !game_dir2xy(dir, &mov_x, &mov_y); // si es 0 es valida

    // posiciones nuevas
    uint x = pirata->posicionX + mov_x;
    uint y = pirata->posicionY + mov_y;

    // si se pasó una dirección válida y la posición nueva es valida
    if(dirValida && game_posicion_valida(x, y)) {
      game_explorar_posicion(pirata, x, y);
    } else {
      game_pirata_exploto();

      return -1;
    }

    return 0;
}

uint game_syscall_cavar(jugador_t *j) {
  pirata_t *pirata = &j->piratas[j->pirataActual];
  uint x = pirata->posicionX;
  uint y = pirata->posicionY;

  if(pirata->tipo == EXPLORADOR) {
    game_pirata_exploto();

    return -1;
  } else {
    // si hay monedas
    if(game_valor_tesoro(x, y) > 0) {
      game_jugador_anotar_punto(j);
      game_minar_botin(x, y);
    } else {
      // Lo hago explotar y libero el slot TODO
      // cuando termina de minar, automáticamente lo cambio por un explorador
      game_pirata_relanzar(pirata, j, EXPLORADOR);

      return -1;
    }
  }

  return 0;
}

uint game_syscall_pirata_posicion(jugador_t *j, int idx) {
  pirata_t *pirata;

  if(idx == -1) {
    pirata = &j->piratas[j->pirataActual];
  } else if (idx >= 0 && idx <= 7){
    pirata = &j->piratas[idx];
  } else {
    game_pirata_exploto();
  }

  return pirata->posicionY << 8 | pirata->posicionX;
}

uint game_syscall_manejar(uint syscall, uint param1) {
  jugador_t *jugadorActual = (jugadorA.activo) ? &jugadorA : &jugadorB;

  if (syscall == MOVERSE) {
    return game_syscall_pirata_mover(jugadorActual, param1);
  }

  if (syscall == CAVAR) {
    return game_syscall_cavar(jugadorActual);
  }

  if (syscall == POSICION) {
    return game_syscall_pirata_posicion(jugadorActual, param1);
  }

  // si se llamó con un parámetro erróneo
  game_pirata_exploto();

  return -1;
}

void game_pirata_exploto() {

}

pirata_t* game_pirata_en_posicion(uint x, uint y) {
	return NULL;
}

void game_jugador_anotar_punto(jugador_t *j) {
  j->monedas += 1; // sumaPunto()
}

void game_terminar_si_es_hora() {
}

/* Resta un botin de la posicion x, y */
void game_minar_botin(uint x, uint y) {
  int i;
  for (i = 0; i < BOTINES_CANTIDAD; i++) {
    if (botines[i][0] == x && botines[i][1] == y) {
      botines[i][2]--; break;
    }
  }
}

uint game_lineal2virtual(uint lineal) {
  return lineal*0x1000 + 0x800000;
}

uint game_lineal2physical(uint lineal) {
  return lineal*0x1000 + 0x500000;
}

void game_actualizar_codigo(uint x0, uint y0, uint x1, uint y1) {
  uint posLineal;

  posLineal = game_xy2lineal(x0, y0);
  // convertimos a posición en memoria
  uint posActual = game_lineal2virtual(posLineal);

  posLineal = game_xy2lineal(x1, y1);
  // convertimos a posición en memoria
  uint posDest = game_lineal2physical(posLineal);

  // actualizamos la posición del código en la dirección 0x400000
  mmu_mapear_pagina(0x400000, rcr3(), posDest, RW);

  // copia el codigo a su nueva posicion
  copiarPagina((unsigned int*) 0x400000, (unsigned int*) posActual);
}

#define KB_w        0x11 // 0x91
#define KB_s        0x1f // 0x9f
#define KB_a        0x1e // 0x9e
#define KB_d        0x20 // 0xa0
#define KB_e        0x12 // 0x92
#define KB_q        0x10 // 0x90
#define KB_i        0x17 // 0x97
#define KB_k        0x25 // 0xa5
#define KB_j        0x24 // 0xa4
#define KB_l        0x26 // 0xa6
#define KB_shiftL   0x2a // 0xaa
#define KB_shiftR   0x36 // 0xb6

void game_atender_teclado(unsigned char tecla) {
  switch(tecla) {

    case KB_w:
      print("     w", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_a:
      print("     a", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_s:
      print("     s", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_d:
      print("     d", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_e:
      print("     e", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_q:
      print("     q", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_i:
      print("     i", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_k:
      print("     k", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_j:
      print("     j", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_l:
      print("     l", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_shiftL:
      print("ShiftL", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_shiftR:
      print("ShiftR", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;

  }

}
