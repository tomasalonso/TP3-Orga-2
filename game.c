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

#define EXPLORADOR 0
#define MINERO 1

uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {30,  3, 50}, {31, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {49,  3, 50}, {48, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };

jugador_t jugadorA;
jugador_t jugadorB;



void* error()
{
	__asm__ ("int3");
	return 0;
}

uint game_xy2lineal (uint x, uint y) {
	return (y * MAPA_ANCHO + x);
}

uint game_posicion_valida(int x, int y) {
	return (x >= 0 && y >= 0 && x < MAPA_ANCHO && y < MAPA_ALTO);
}

pirata_t* id_pirata2pirata(uint id_pirata)
{
    // ~ completar ~
	return NULL;
}

uint game_dir2xy(direccion dir, int *x, int *y)
{
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

uint game_valor_tesoro(uint x, uint y)
{
	int i;
	for (i = 0; i < BOTINES_CANTIDAD; i++)
	{
		if (botines[i][0] == x && botines[i][1] == y)
			return botines[i][2];
	}
	return 0;
}

// dada una posicion (x,y) guarda las posiciones de alrededor en dos arreglos, uno para las x y otro para las y
void game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y)
{
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


void game_inicializar()
{
}

void game_jugador_inicializar_mapa(jugador_t *jug)
{
}

void game_jugador_inicializar(jugador_t *j)
{
	static int index = 0;

	j->index = index++;
    // ~ completar ~

}

void game_pirata_inicializar(pirata_t *pirata, jugador_t *j, uint index, uint id)
{
}

// Ejercicio 5.b
void game_tick(uint id_pirata)
{
  screen_actualizar_reloj_global();
}


void game_pirata_relanzar(pirata_t *pirata, jugador_t *j, uint tipo)
{
}

pirata_t* game_jugador_erigir_pirata(jugador_t *j, uint tipo)
{
    // ~ completar ~

	return NULL;
}


void game_jugador_lanzar_pirata(jugador_t *j, uint tipo, int x, int y)
{
}

void game_pirata_habilitar_posicion(jugador_t *j, pirata_t *pirata, int x, int y)
{
}


void game_explorar_posicion(jugador_t *jugador, int c, int f)
{
}


uint game_syscall_pirata_mover(direccion dir)
{
    pirata_t *pirAux = NULL;

    if(jugadorA.activo){
        pirAux = &jugadorA.piratas[jugadorA.pirataActual];
    } else {
        pirAux = &jugadorB.piratas[jugadorB.pirataActual];
    }

    int x;
    int y;

    uint dirValida = game_dir2xy(dir, &x, &y);

    // si se pasó una dirección válida y la posición nueva es valida
    if(dirValida == 0 && game_posicion_valida(pirAux->posicionX+x, pirAux->posicionY+y)) {
      uint posLineal;

      posLineal = game_xy2lineal(pirAux->posicionX, pirAux->posicionY);
      // convertimos a posición en memoria
      unsigned int posActual = posLineal*0x1000 + 0x800000;

      pirAux->posicionX += x;
      pirAux->posicionY += y;

      posLineal = game_xy2lineal(pirAux->posicionX, pirAux->posicionY);
      // convertimos a posición en memoria
      uint posDest = posLineal*0x1000 + 0x800000;

      copiarPagina((unsigned int*) posDest, (unsigned int*) posActual);
    } else {
      game_pirata_exploto();

      return -1;
    }

    return 0;
}

uint game_syscall_cavar()
{
  pirata_t *pirAux;
  jugador_t *jugadorActual;

  if(jugadorA.activo){
    jugadorActual = &jugadorA;
  } else {
    jugadorActual = &jugadorB;
  }
  pirAux = &jugadorActual->piratas[jugadorActual->pirataActual];

  if(pirAux->tipo == EXPLORADOR) {
    game_pirata_exploto();
  } else {
    // si hay monedas
    if(game_valor_tesoro(pirAux->posicionX, pirAux->posicionY) > 0) {
      jugadorActual->monedas += 1; // sumaPunto()

      // resta un botín de la posición
      int i;
    	for (i = 0; i < BOTINES_CANTIDAD; i++) {
    		if (botines[i][0] == pirAux->posicionX && botines[i][1] == pirAux->posicionY) {
    		  botines[i][2]--; break;
    	  }
      }
    } else {
      //cuando termina de minar, automáticamente lo cambio por un explorador
      game_pirata_relanzar(pirAux, jugadorActual, EXPLORADOR);
    }
  }

  return 0;
}

uint game_syscall_pirata_posicion(int idx)
{
  jugador_t *jugadorActual;
  pirata_t *pirAux;

  if(jugadorA.activo){
      jugadorActual = &jugadorA;
  } else {
      jugadorActual = &jugadorB;
  }

  if(idx == -1) {
    pirAux = &jugadorActual->piratas[jugadorActual->pirataActual];
  } else { //asumimos que idx esta bien definido
    pirAux = &jugadorActual->piratas[idx];
  }

  return pirAux->posicionY << 8 | pirAux->posicionX;
}

uint game_syscall_manejar(uint syscall, uint param1)
{
  /* if (syscall == MOVERSE) { */
    
  /* } else if (syscall == CAVAR) { */
    
  /* } else { */
    
  /* } */
    return 0;
}

void game_pirata_exploto()
{
}

pirata_t* game_pirata_en_posicion(uint x, uint y)
{
	return NULL;
}


void game_jugador_anotar_punto(jugador_t *j)
{
}



void game_terminar_si_es_hora()
{
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


void game_atender_teclado(unsigned char tecla)
{
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
