/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"
#include "game.h"
#include "i386.h"


extern jugador_t jugadorA, jugadorB;


static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;

const char reloj[] = "|/-\\";
#define reloj_size 4

#define INI_F_MAPA 1
#define FIN_F_MAPA 44
#define INI_C_MAPA 0
#define FIN_C_MAPA 79

#define INI_F_PIE 45
#define FIN_F_PIE 49

#define INI_C_ROJO 33
#define FIN_C_ROJO 39

#define INI_C_AZUL 40
#define FIN_C_AZUL 46

#define INI_C_RELOJ_ROJO 3
#define INI_C_RELOJ_AZUL 57

void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);
}

void screen_pintar(uchar c, uchar color, uint fila, uint columna)
{
    p[fila][columna].c = c;
    p[fila][columna].a = color;
}

uchar screen_valor_actual(uint fila, uint columna)
{
    return p[fila][columna].c;
}

void print(const char * text, uint x, uint y, unsigned short attr) {
    int i;
    for (i = 0; text[i] != 0; i++)
     {
        screen_pintar(text[i], attr, y, x);

        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void print_hex(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

void print_dec(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        int resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}


// Ejercicio 3.a
void screen_inicializar() {
  /* Toda la pantalla a gris */
  screen_pintar_rect(0, C_BG_LIGHT_GREY, INI_F_MAPA, INI_C_MAPA, 44, 80);

  /* Fila negra */
  screen_pintar_linea_h(0, C_BG_BLACK, 0, 0, 80);

  /* Cuadros rojo y azul para los jugadores */
  /* Cuadro rojo */
  screen_pintar_rect(0, C_BG_RED, INI_F_PIE, INI_C_ROJO, 5, 7);

  /* Cuadro azul */
  screen_pintar_rect(0, C_BG_BLUE, INI_F_PIE, INI_C_AZUL, 5, 7);

  /* Pintar puntajes */
  screen_pintar_puntajes();

  /* Pintar relojes vacíos */
  screen_pintar_relojes();

  /* Ejercicio 3.d Escribir nombre del grupo en pantalla */
  print("| Pencylvester estudia en MIT |", 80-32-10, 0, C_FG_WHITE);
}

// Ejercicio 3.a
void screen_pintar_rect(unsigned char c, unsigned char color, int fila, int columna, int alto, int ancho) {
  int i, j;

  for (i = columna; i < columna+ancho && i < 80; i++) {
    for (j = fila; j < fila+alto && j < 50; j++) {
      screen_pintar(c, color, j, i);
    }
  }
}

// Ejercicio 3.a
void screen_pintar_linea_h(unsigned char c, unsigned char color, int fila, int columna, int ancho) {
  screen_pintar_rect(c, color, fila, columna, 1, ancho);
}

// Ejercicio 3.a
void screen_pintar_linea_v(unsigned char c, unsigned char color, int fila, int columna, int alto) {
  screen_pintar_rect(c, color, fila, columna, alto, 1);
}

// Ejercicio 3.a
void screen_pintar_puntajes() {
  print("0", INI_C_ROJO+2, INI_F_PIE+2, (C_FG_WHITE | C_BG_RED));
  print("0", INI_C_AZUL+2, INI_F_PIE+2, (C_FG_WHITE | C_BG_BLUE));
}

unsigned char screen_color_jugador(jugador_t *j) {
  return 0;
}

unsigned char screen_caracter_pirata(unsigned int tipo) {
  return 0;
}

void screen_pintar_pirata(jugador_t *j, pirata_t *pirata) {
  unsigned int x = pirata->posicionX;
  unsigned int y = pirata->posicionY;
  unsigned char tipo = (pirata->tipo == EXPLORADOR) ? 'E' : 'M';

  /* Prueba */
  /*
  x = y = 10;
  tipo = 'E';
  */
  screen_pintar(tipo, screen_color_jugador(j), y+1, x);
  screen_pintar_rect_color(C_BG_GREEN, y, x-1, 3, 3);
  /* Fin prueba */

  /* screen_pintar_rect_color(screen_color_jugador(j), y-1, x-1, 3, 3); */
  /* screen_pintar(screen_caracter_pirata(tipo), screen_color_jugador(j), y, x); */
}

void screen_borrar_pirata(jugador_t *j, pirata_t *pirata) {

}

void screen_actualizar_reloj_pirata (jugador_t *j, pirata_t *pirata) {

}

void screen_pintar_reloj_pirata(jugador_t *j, pirata_t *pirata) {

}

void screen_pintar_reloj_piratas(jugador_t *j) {

}

void screen_pintar_relojes() {
  int i;

  for (i = 1; i <= 8; i++) {
    // Transforma int a ascii
    screen_pintar(i+48, (C_FG_WHITE | C_BG_BLACK), INI_F_PIE+1, INI_C_RELOJ_ROJO+i*2);
    screen_pintar('x', (C_FG_RED | C_BG_BLACK), INI_F_PIE+3, INI_C_RELOJ_ROJO+i*2);
    screen_pintar(i+48, (C_FG_WHITE | C_BG_BLACK), INI_F_PIE+1, INI_C_RELOJ_AZUL+i*2);
    screen_pintar('x', (C_FG_RED | C_BG_BLACK), INI_F_PIE+3, INI_C_RELOJ_AZUL+i*2);
  }
}

void screen_actualizar_posicion_mapa(uint x, uint y) {

}

void screen_stop_game_show_winner(jugador_t *j) {

}

// Ejercicio 3.a
void screen_pintar_rect_color(unsigned char color, int fila, int columna, int alto, int ancho) {
  int i, j;

  for (i = columna; i < columna+ancho && i < 80; i++) {
    for (j = fila; j < fila+alto && j < 50; j++) {
      p[j][i].a = color;
    }
  }
}
