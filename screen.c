/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"
#include "game.h"
#include "sched.h"
#include "i386.h"


extern jugador_t jugadorA, jugadorB;


static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;
static ca pantalla[44][80];


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
  print_dec(jugadorA.monedas, 3, INI_C_ROJO+2, INI_F_PIE+2, (C_FG_WHITE | C_BG_RED));
  print_dec(jugadorB.monedas, 3, INI_C_AZUL+2, INI_F_PIE+2, (C_FG_WHITE | C_BG_BLUE));
}

unsigned char screen_color_jugador(jugador_t *j) {
  return j->color;
}

unsigned char screen_caracter_pirata(unsigned int tipo) {
  return (tipo == EXPLORADOR) ? 'E' : 'M';
}

void screen_pintar_pirata(jugador_t *j, pirata_t *pirata, uint dir) {
  unsigned int x = pirata->posicionX;
  unsigned int y = pirata->posicionY;
  unsigned char tipo = screen_caracter_pirata(pirata->tipo);

  uint antX;
  uint antY;
  switch (dir)
    {
		case IZQ:  antX = x+1; antY =   y; break;
		case DER:  antX = x-1; antY =   y; break;
		case ABA:  antX =   x; antY = y-1; break;
		case ARR:  antX =   x; antY = y+1; break;
    case TODO: break;
    }

  if (dir != TODO) {
    screen_pintar(tipo, screen_color_jugador(j), antY+1, antX);
  }
  screen_pintar(tipo, (screen_color_jugador(j)+(3 << 4)) | C_FG_WHITE, y+1, x);
}

void screen_matar_pirata(pirata_t *pirata) {
  uint x = (pirata->jugador->index == JUGADOR_A) ? INI_C_RELOJ_ROJO : INI_C_RELOJ_AZUL;
  uint i = pirata->index+1;

  screen_pintar_rect_color(screen_color_jugador(pirata->jugador), pirata->posicionY+1, pirata->posicionX, 1, 1);
  screen_pintar('x', screen_color_jugador(pirata->jugador) | C_FG_RED, pirata->posicionY+1, pirata->posicionX);

  screen_pintar('x', C_FG_RED, INI_F_PIE+3, x+i*2);
}
void screen_actualizar_reloj_pirata (jugador_t *j, pirata_t *pirata) {
}

void screen_pintar_reloj_pirata(jugador_t *j, pirata_t *pirata) {
  static uint reloj_pirata[2][8] = {{0}};

  uint *r = &reloj_pirata[j->index][pirata->index];
  uint i = pirata->index+1;

  *r = (*r + 1) % reloj_size;

  uint x = (j->index == JUGADOR_A) ? INI_C_RELOJ_ROJO : INI_C_RELOJ_AZUL;
  screen_pintar(reloj[*r], C_BW, INI_F_PIE+3, x+i*2);
}

void screen_pintar_reloj_piratas(jugador_t *j) {
  int i;
  for (i = 0; i < 8; i++) {
    pirata_t *p = &j->piratas[i];

    if (sched_pirata_activo(p)) {
      screen_pintar_reloj_pirata(j, p);
    }
  }
}

void screen_pintar_relojes() {
  int i;

  for (i = 1; i <= 8; i++) {
    // Transforma int a ascii
    screen_pintar(i+48, (C_FG_WHITE | C_BG_BLACK), INI_F_PIE+1, INI_C_RELOJ_ROJO+i*2);
    screen_pintar('x', (C_FG_WHITE | C_BG_BLACK), INI_F_PIE+3, INI_C_RELOJ_ROJO+i*2);
    screen_pintar(i+48, (C_FG_WHITE | C_BG_BLACK), INI_F_PIE+1, INI_C_RELOJ_AZUL+i*2);
    screen_pintar('x', (C_FG_WHITE | C_BG_BLACK), INI_F_PIE+3, INI_C_RELOJ_AZUL+i*2);
  }
}

void screen_actualizar_posicion_mapa(uint x, uint y) {

}

void screen_stop_game_show_winner(jugador_t *j) {
  if (j == NULL) {
    // empate
  } else {

  }
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

void screen_pintar_botin(uint x, uint y) {
  screen_pintar('@', C_BG_LIGHT_GREY|C_FG_RED, y+1, x);
}

#define DEBUG_INI_X 24
#define DEBUG_INI_Y 6
#define DEBUG_ANCHO 32
#define DEBUG_ALTO 36
#define DEBUG_INI_REG_X DEBUG_INI_X+2
#define DEBUG_INI_REG_Y DEBUG_INI_Y+3

void screen_debug(char *exc,
                  uint cs, uint ss, uint ds,
                  uint es, uint fs, uint gs,
                  uint esp, uint eip, uint eflags, uint *greg) {
  /* print_hex(cs, 20, 0, 0, 0xF0); */
  /* print_hex(ss, 20, 0, 1, 0xF0); */
  /* print_hex(ds, 20, 0, 2, 0xF0); */
  /* print_hex(es, 20, 0, 3, 0xF0); */
  /* print_hex(fs, 20, 0, 4, 0xF0); */
  /* print_hex(gs, 20, 0, 5, 0xF0); */
  /* print_hex(esp, 20, 0, 6, 0xF0); */
  /* print_hex(eip, 20, 0, 7, 0xF0); */
  /* print_hex(eflags, 20, 0, 8, 0xF0); */
  /* print_hex((unsigned int)greg, 20, 0, 9, 0xF0); */

  screen_pintar_rect_color(C_BG_BLACK|C_FG_BLACK, DEBUG_INI_Y, DEBUG_INI_X, DEBUG_ALTO, DEBUG_ANCHO);
  screen_pintar_rect_color(C_BG_LIGHT_GREY|C_FG_LIGHT_GREY, DEBUG_INI_Y+1, DEBUG_INI_X+1, DEBUG_ALTO-2, DEBUG_ANCHO-2);
  screen_pintar_rect_color(C_BG_RED|C_FG_RED, DEBUG_INI_Y+1, DEBUG_INI_X+1, 1, DEBUG_ANCHO-2);

  print(exc, DEBUG_INI_X+2, DEBUG_INI_Y+1, C_BG_RED | C_FG_WHITE);

  print("eax", DEBUG_INI_REG_X, DEBUG_INI_REG_Y, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("ebx", DEBUG_INI_REG_X, DEBUG_INI_REG_Y+2, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("ecx", DEBUG_INI_REG_X, DEBUG_INI_REG_Y+4, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("edx", DEBUG_INI_REG_X, DEBUG_INI_REG_Y+6, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("esi", DEBUG_INI_REG_X, DEBUG_INI_REG_Y+8, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("edi", DEBUG_INI_REG_X, DEBUG_INI_REG_Y+10, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("ebp", DEBUG_INI_REG_X, DEBUG_INI_REG_Y+12, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("esp", DEBUG_INI_REG_X, DEBUG_INI_REG_Y+14, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("eip", DEBUG_INI_REG_X, DEBUG_INI_REG_Y+16, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("cs", DEBUG_INI_REG_X+1, DEBUG_INI_REG_Y+18, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("ds", DEBUG_INI_REG_X+1, DEBUG_INI_REG_Y+20, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("es", DEBUG_INI_REG_X+1, DEBUG_INI_REG_Y+22, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("fs", DEBUG_INI_REG_X+1, DEBUG_INI_REG_Y+24, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("gs", DEBUG_INI_REG_X+1, DEBUG_INI_REG_Y+26, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("ss", DEBUG_INI_REG_X+1, DEBUG_INI_REG_Y+28, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("eflags", DEBUG_INI_REG_X+1, DEBUG_INI_REG_Y+30, C_BG_LIGHT_GREY|C_FG_BLACK);

  print("cr0", DEBUG_INI_REG_X+16, DEBUG_INI_REG_Y, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("cr2", DEBUG_INI_REG_X+16, DEBUG_INI_REG_Y+2, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("cr3", DEBUG_INI_REG_X+16, DEBUG_INI_REG_Y+4, C_BG_LIGHT_GREY|C_FG_BLACK);
  print("cr4", DEBUG_INI_REG_X+16, DEBUG_INI_REG_Y+6, C_BG_LIGHT_GREY|C_FG_BLACK);

  print("stack", DEBUG_INI_REG_X+16, DEBUG_INI_REG_Y+17, C_BG_LIGHT_GREY|C_FG_BLACK);

  // Registros de propósito general
  uint *eax = greg+7;
  uint *ebx = greg+4;
  uint *ecx = greg+6;
  uint *edx = greg+5;
  uint *esi = greg+1;
  uint *edi = greg;
  uint *ebp = greg+2;

  print_hex(*eax, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(*ebx, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+2, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(*ecx, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+4, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(*edx, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+6, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(*esi, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+8, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(*edi, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+10, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(*ebp, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+12, C_BG_LIGHT_GREY|C_FG_WHITE);

  print_hex(esp, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+14, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(eip, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+16, C_BG_LIGHT_GREY|C_FG_WHITE);

  // Registro de segmento 6
  print_hex(cs, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+18, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(ds, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+20, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(es, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+22, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(fs, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+24, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(gs, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+26, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(ss, 8, DEBUG_INI_REG_X+5, DEBUG_INI_REG_Y+28, C_BG_LIGHT_GREY|C_FG_WHITE);


  // Registro EFLAGS
  print_hex(eflags, 8, DEBUG_INI_REG_X+8, DEBUG_INI_REG_Y+30, C_BG_LIGHT_GREY|C_FG_WHITE);

  // Registros de control
  print_hex(rcr0(), 8, DEBUG_INI_REG_X+20, DEBUG_INI_REG_Y, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(rcr2(), 8, DEBUG_INI_REG_X+20, DEBUG_INI_REG_Y+2, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(rcr3(), 8, DEBUG_INI_REG_X+20, DEBUG_INI_REG_Y+4, C_BG_LIGHT_GREY|C_FG_WHITE);
  print_hex(rcr4(), 8, DEBUG_INI_REG_X+20, DEBUG_INI_REG_Y+6, C_BG_LIGHT_GREY|C_FG_WHITE);

  // Primeros 5 de la pila
  int i;
  for (i = 0; i < 5; i++) {
    print_hex(*((uint *) esp+i), 8, DEBUG_INI_REG_X+16, DEBUG_INI_REG_Y+20+i, C_BG_LIGHT_GREY|C_FG_WHITE);
  }
}

void screen_guardar() {
  int i, j;
  for (i = 0; i < DEBUG_ALTO; i++) {
    for (j = 0; j < DEBUG_ANCHO; j++) {
      pantalla[i][j] = p[DEBUG_INI_Y+i][DEBUG_INI_X+j];
    }
  }
}

void screen_cargar() {
  int i, j;
  for (i = 0; i < DEBUG_ALTO; i++) {
    for (j = 0; j < DEBUG_ANCHO; j++) {
      p[DEBUG_INI_Y+i][DEBUG_INI_X+j] = pantalla[i][j];
    }
  }
}
