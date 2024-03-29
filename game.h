/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#ifndef __GAME_H__
#define __GAME_H__

#include "defines.h"

typedef enum direccion_e { ARR = 0x4, ABA = 0x7, DER = 0xA, IZQ = 0xD, TODO = 0xF} direccion;

#define BOTINES_CANTIDAD 8

#define MAX_CANT_PIRATAS_VIVOS           8

#define JUGADOR_A                         0
#define JUGADOR_B                         1

#define MAPA_ANCHO                       80
#define MAPA_ALTO                        44

// Ejercicio 4.b
// esquinas superior izquierda e inferior derecha
#define PUERTO_A  0x500000
#define PUERTO_B  0x12C0000

#define VIRT_INI_MAPA  0x800000
#define FIS_INI_MAPA   0x500000

// distancia entre las posiciones en el mapa (4kb)
#define OFFSET_COL 0x1000       //(4kb)
#define OFFSET_FILA 0x1000*0x50 //(4kb*80)

#define TASK_AE  0x10000
#define TASK_AM  0x11000
#define TASK_BE  0x12000
#define TASK_BM  0x13000

#define EXPLORADOR 0
#define MINERO 1

#define LIBRE 0
#define EJECUCION 1
#define MUERTO -1


struct jugador_t;

typedef struct pirata_t
{
    uint index;
    struct jugador_t *jugador;
    uint posicionX;
    uint posicionY;
    uint tipo;

    // id unica, posicion, tipo, reloj
} pirata_t;


typedef struct jugador_t
{
    uint index;
    pirata_t piratas[MAX_CANT_PIRATAS_VIVOS];
    uint monedas;
    uint color;
    // Agregadas al hacer Ejercicio 6.g
    uint mapa[4]; // direcciones a las page table del mapa
    uint puertoX;
    uint puertoY;

    uint codigo[2]; // dirección al código de las tareas

    uint botin[BOTINES_CANTIDAD]; // botines encontrados

    // coordenadas puerto, posiciones exploradas, mineros pendientes, etc
} jugador_t;


extern jugador_t jugadorA, jugadorB;

// ~ auxiliares dadas ~
uint game_xy2lineal(uint x, uint y);

// ~ auxiliares sugeridas o requeridas (segun disponga enunciado) ~
void game_pirata_inicializar(jugador_t *j);
void game_pirata_erigir(pirata_t *pirata, jugador_t *j, uint tipo);
void game_pirata_habilitar_posicion(uint x, uint y, uint cr3);
void game_pirata_exploto();

void game_jugador_inicializar(jugador_t *j);
void game_jugador_lanzar_pirata(jugador_t *j, uint tipo, int x, int y);
pirata_t* game_jugador_erigir_pirata(jugador_t *j, uint tipo);
void game_jugador_anotar_punto(jugador_t *j);
void game_explorar_posicion(pirata_t *pirata, uint pd, uint x, uint y, direccion dir);
uint game_posicion_valida(int x, int y);

uint game_valor_tesoro(uint x, uint y);
int game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y, direccion dir);

uint game_syscall_pirata_mover(pirata_t *pirata, direccion dir);
uint game_syscall_cavar(jugador_t *j, pirata_t *pirata);
uint game_syscall_pirata_posicion(jugador_t *j, int idx);
int game_syscall_manejar(uint syscall, uint param1);
void game_tick();
void game_terminar_si_es_hora();
void game_atender_teclado(unsigned char tecla);

/* Nuestras */

/* Resta un botin de la posicion x, y */
void game_minar_botin(uint x, uint y);
uint game_lineal2virtual(uint posLineal);
void game_actualizar_codigo(uint x0, uint y0, uint x1, uint y1);
uint game_lineal2physical(uint lineal);
void game_jugador_inicializar_mapa(jugador_t *j);
void game_pirata_mover(pirata_t *pirata, uint x, uint y, direccion dir);
void game_jugador_lanzar_minero(jugador_t *j, int x, int y);
void game_jugador_lanzar_explorador(jugador_t *j);
uint game_calcular_fin();
void game_atender_excepcion(char *exc,
                            uint cs, uint ss, uint ds,
                            uint es, uint fs, uint gs,
                            uint esp, uint eip, uint eflags, uint *greg);
uint game_nro_botin(uint x, uint y);
void game_jugador_botin_inicializar(jugador_t *j);
void game_mineros_pendientes(jugador_t *j);

#endif  /* !__GAME_H__ */
