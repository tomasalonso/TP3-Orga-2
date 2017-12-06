/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "tss.h"
#include "screen.h"
#include "sched.h"
#include "mmu.h"

#include <stdarg.h>


#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      1
#define POS_INIT_B_X         MAPA_ANCHO - 2
#define POS_INIT_B_Y          MAPA_ALTO - 2

#define CANT_POSICIONES_VISTAS            9
#define MAX_SIN_CAMBIOS                 999

// Para syscall 0x46
#define MOVERSE 0x1
#define CAVAR 0x2
#define POSICION 0x3

// Para finalizar el juego
#define FIN 0x10

uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
  {30,  3, 50}, {31, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
  {49,  3, 50}, {48, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };

jugador_t jugadorA;
jugador_t jugadorB;


uint puntajeA;
uint puntajeB;
uint contador;

uint debug;

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
int game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y, direccion dir) {
  // pre: dir valida
  int iniX, finX;
  int iniY, finY;

  switch (dir)
    {
		case IZQ:  iniX = -1; finX = -1; iniY = -1; finY =  1; break;
		case DER:  iniX =  1; finX =  1; iniY = -1; finY =  1; break;
		case ABA:  iniX = -1; finX =  1; iniY =  1; finY =  1; break;
		case ARR:  iniX = -1; finX =  1; iniY = -1; finY = -1; break;
		case TODO: iniX = -1; finX =  1; iniY = -1; finY =  1; break;
    }

	int next = 0;
	int i, j;
	for (i = iniY; i <= finY; i++)
    {
      for (j = iniX; j <= finX; j++)
        {
          vistas_x[next] = x + j;
          vistas_y[next] = y + i;
          next++;
        }
    }

  return next;
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

  /* Inicializamos contadores para
     finalizar juego */
  puntajeA = 0;
  puntajeB = 0;
  contador = 0;

  /* Debug desactivado por defecto */
  debug = 0;
}

void game_jugador_inicializar(jugador_t *j) {
	static int index = 0;

	j->index = index++;  // asigna 0, 1, 2, 3, ...
  game_pirata_inicializar(j);
  j->monedas = 0;
  game_jugador_botin_inicializar(j);

  if (j->index == JUGADOR_A) {
    // JUGADOR_A
    j->color = C_BG_GREEN;
    j->codigo[EXPLORADOR] = TASK_AE;
    j->codigo[MINERO] = TASK_AM;
    j->puertoX = POS_INIT_A_X;
    j->puertoY = POS_INIT_A_Y;
  } else {
    // JUGADOR_B
    j->color = C_BG_MAGENTA;
    j->codigo[EXPLORADOR] = TASK_BE;
    j->codigo[MINERO] = TASK_BM;
    j->puertoX = POS_INIT_B_X;
    j->puertoY = POS_INIT_B_Y;
  }

  game_jugador_inicializar_mapa(j);
}

void game_jugador_botin_inicializar(jugador_t *j) {
  int i;
  for (i = 0; i < BOTINES_CANTIDAD; i++) {
    j->botin[i] = 0;
  }
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
  }
}

// Ejercicio 5.b
void game_tick() {
  screen_pintar_reloj_piratas(&jugadorA);
  screen_pintar_reloj_piratas(&jugadorB);
  screen_actualizar_reloj_global();
}

void game_jugador_lanzar_pirata(jugador_t *j, uint tipo, int x, int y) {
  uint slot = sched_proximo_slot_libre(j->index);
  pirata_t *pirata = &j->piratas[slot];

  pirata->tipo = tipo;
  pirata->posicionX = j->puertoX;
  pirata->posicionY = j->puertoY;

  inicializar_tss_pirata(pirata, mmu_inicializar_dir_pirata(pirata, x, y));

  screen_pintar_pirata(j, pirata, TODO);
}

void game_jugador_lanzar_minero(jugador_t *j, int x, int y) {
  game_jugador_lanzar_pirata(j, MINERO, x, y);
}

void game_jugador_lanzar_explorador(jugador_t *j) {
  game_jugador_lanzar_pirata(j, EXPLORADOR, 0, 0);
}

void game_explorar_posicion(pirata_t *pirata, uint pd, uint x, uint y, direccion dir) {
  // mapea el mapa, cuak
  int v_x[9];
  int v_y[9];

  int cant = game_calcular_posiciones_vistas(v_x, v_y, x, y, dir);
  // mapeamos las posiciones del mapa nuevas
  int i;
  for (i = 0; i < cant; i++) {
    if (game_posicion_valida(v_x[i], v_y[i])) {
      game_pirata_habilitar_posicion(v_x[i], v_y[i], pd);
      screen_pintar_rect_color(screen_color_jugador(pirata->jugador), v_y[i]+1, v_x[i], 1, 1);
      // Si hay botin, a minar!
      if (game_valor_tesoro(v_x[i], v_y[i])) {
        if (sched_hay_slot_libre(pirata->jugador->index)) {
          game_jugador_lanzar_minero(pirata->jugador, v_x[i], v_y[i]);
        } else {
          pirata->jugador->botin[game_nro_botin(v_x[i], v_y[i])] = 1;
        }
        screen_pintar_botin(v_x[i], v_y[i]);
      }
    }
  }
}

void game_pirata_habilitar_posicion(uint x, uint y, uint cr3) {
  uint posLineal = game_xy2lineal(x, y);
  // convertimos a posición en memoria virtual y física
  uint virtual = game_lineal2virtual(posLineal);
  uint fisica = game_lineal2physical(posLineal);

  // mapeamos la posición del mapa
  // (tranquilamente podría ya haber sido mapeada
  //  por otro explorador)
  mmu_mapear_pagina(virtual, cr3, fisica, RO);
}

void game_pirata_mover(pirata_t *pirata, uint x, uint y, direccion dir) {
  // copia el codigo en su nueva posicion
  game_actualizar_codigo(pirata->posicionX, pirata->posicionY, x, y);

  // actualizamos la posición del pirata
  pirata->posicionX = x;
  pirata->posicionY = y;

  screen_pintar_pirata(pirata->jugador, pirata, (unsigned int) dir);
}

uint game_syscall_pirata_mover(pirata_t *pirata, direccion dir) {
    int mov_x, mov_y;

    int dirValida = !game_dir2xy(dir, &mov_x, &mov_y); // si es 0 es valida

    // posiciones nuevas
    int x = pirata->posicionX + mov_x;
    int y = pirata->posicionY + mov_y;

    // si se pasó una dirección válida y la posición nueva es valida
    if(dirValida && game_posicion_valida(x, y)) {
      if (pirata->tipo == EXPLORADOR) {
        game_explorar_posicion(pirata, rcr3(), x, y, dir);
      }
      game_pirata_mover(pirata, x, y, dir);
    } else {
      game_pirata_exploto();

      return -1;
    }

    return 0;
}

uint game_syscall_cavar(jugador_t *j, pirata_t *pirata) {
  uint x = pirata->posicionX;
  uint y = pirata->posicionY;

  if (pirata->tipo == EXPLORADOR || game_valor_tesoro(x, y) <= 0) {
    // si algo esta mal
    game_pirata_exploto();
    return -1;
  }

  // si hay monedas
  game_jugador_anotar_punto(j);
  game_minar_botin(x, y);
  screen_pintar_puntajes();

  return 0;
}

uint game_syscall_pirata_posicion(jugador_t *j, int idx) {
  pirata_t *pirata = NULL;

  if (idx < -1 || idx > 7) {
    // si algo esta mal
    game_pirata_exploto();
    return -1;
  }

  if (idx == -1) {
    pirata = sched_pirata_actual();
  } else {
    pirata = &j->piratas[idx];
  }

  return pirata->posicionY << 8 | pirata->posicionX;
}

int game_syscall_manejar(uint syscall, uint param1) {
  jugador_t *jugador = sched_jugador_actual();
  pirata_t *pirata = sched_pirata_actual();

  if (syscall == MOVERSE) {
    return game_syscall_pirata_mover(pirata, param1);
  }

  if (syscall == CAVAR) {
    return game_syscall_cavar(jugador, pirata);
  }

  if (syscall == POSICION) {
    return game_syscall_pirata_posicion(jugador, param1);
  }

  // si se llamó con un parámetro erróneo
  game_pirata_exploto();

  return -1;
}

void game_pirata_exploto() {
  pirata_t *p = sched_pirata_actual();

  screen_matar_pirata(p);
  sched_liberar_slot();
}

void game_jugador_anotar_punto(jugador_t *j) {
  j->monedas += 1; // sumaPunto()
}

void game_terminar_si_es_hora() {
  sched_detener();

  if (jugadorA.monedas > jugadorB.monedas)
    screen_stop_game_show_winner(&jugadorA);
  else if (jugadorA.monedas < jugadorB.monedas)
    screen_stop_game_show_winner(&jugadorB);
  else
    screen_stop_game_show_winner(NULL);
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
#define KB_y        0x15 // 0xa7
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
    case KB_y:
      print("     y", 74, 0, C_BG_BLACK | C_FG_WHITE);
      if (debug) {
        if (!sched_activo()) {
          screen_cargar();
          sched_activar();
        } else {
          print("     ", 20, 0, 0x0);
          debug = 0;
        }
      } else {
        print("DEBUG", 20, 0, 0xF0);
        debug = 1;
      }
      break;
    case KB_shiftL:
      if (sched_hay_slot_libre(jugadorA.index)) {
        game_jugador_lanzar_explorador(&jugadorA);
      }
      print("ShiftL", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
    case KB_shiftR:
      if (sched_hay_slot_libre(jugadorB.index)) {
        game_jugador_lanzar_explorador(&jugadorB);
      }
      print("ShiftR", 74, 0, C_BG_BLACK | C_FG_WHITE);
      break;
  }
}

uint game_calcular_fin() {
  if (contador == FIN) {
    game_terminar_si_es_hora();

    return 1;
  }

  if (sched_activo() &&
      !sched_hay_slot_libre(jugadorA.index) &&
      !sched_hay_slot_libre(jugadorB.index) &&
      puntajeA == jugadorA.monedas &&
      puntajeB == jugadorB.monedas) {
    contador++;
  } else {
    contador = 0;
    puntajeA = jugadorA.monedas;
    puntajeB = jugadorB.monedas;
  }

  return 0;
}

void game_atender_excepcion(char *exc,
                            uint cs, uint ss, uint ds,
                            uint es, uint fs, uint gs,
                            uint esp, uint eip, uint eflags, uint *greg) {
  game_pirata_exploto();
  if (debug) {
    sched_detener();
    screen_guardar();
    screen_debug(exc,cs,ss,ds,es,fs,gs,esp,eip,eflags,greg);
  }
}

uint game_nro_botin(uint x, uint y) {
  int i;
  for (i = 0; i < BOTINES_CANTIDAD; i++) {
    if (botines[i][0] == x && botines[i][1] == y) {
      return i;
    }
  }

  return 0;
}

void game_mineros_pendientes(jugador_t *j) {
  if (sched_hay_slot_libre(j->index)) {
    int i;
    for (i = 0; i < BOTINES_CANTIDAD; i++) {
      if (j->botin[i]) {
        game_jugador_lanzar_minero(j, botines[i][0], botines[i][1]);
        j->botin[i] = 0;
        break;
      }
    }
  }
}
