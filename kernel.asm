  ; ** por compatibilidad se omiten tildes **
  ; ==============================================================================
  ; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
  ; ==============================================================================

%include "imprimir.mac"

extern screen_inicializar
extern GDT_DESC
extern IDT_DESC                 ; para inicializar la IDT
global start

;; Saltear seccion de datos
  jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
  iniciando_mr_msg db     'Iniciando kernel (Modo Real)...'
  iniciando_mr_len equ    $ - iniciando_mr_msg

  iniciando_mp_msg db     'Iniciando kernel (Modo Protegido)...'
  iniciando_mp_len equ    $ - iniciando_mp_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
  ; Deshabilitar interrupciones
  cli

  ; Cambiar modo de video a 80 X 50
  mov ax, 0003h
  int 10h ; set mode 03h
  xor bx, bx
  mov ax, 1112h
  int 10h ; load 8x8 font

  ; Imprimir mensaje de bienvenida
  imprimir_texto_mr iniciando_mr_msg, iniciando_mr_len, 0x07, 0, 0


  ; Habilitar A20
  call habilitar_A20

  ; Cargar la GDT
  lgdt [GDT_DESC]

  ; Setear el bit PE del registro CR0
  mov eax, CR0
  or eax, 1                   ; es el bit 0
  mov CR0, eax

  ; Saltar a modo protegido
  jmp (0x8 << 3):mp           ; carga el cs con 8:000

BITS 32
mp:
  ; Establecer selectores de segmentos
  xor eax, eax
%define SELECTOR_CODE_ROOT 0x09 << 3
  mov eax, SELECTOR_CODE_ROOT
  mov ds, ax                  ; segmento de datos
  mov ss, ax                  ; segmento de pila, apunta igual que el segmento de datos
  mov es, ax
  mov gs, ax
  mov fs, ax

  ; Establecer la base de la pila
  mov esp, 0x27000
  mov ebp, 0x27000

  ; Ejercicio 1.d
  ; mov eax, (12 << 3)
  ; mov ds, ax                  ; segmento de datos, apunta a segmento de video
  ; ; Borramos algunas letras de prueba
  ; mov byte [0x00], 0
  ; mov byte [0x01], 0
  ; mov byte [0x02], 0
  ; mov byte [0x03], 0
  ; mov byte [0x04], 0
  ; mov byte [0x05], 0
  ; mov byte [0x06], 0
  ; mov byte [0x07], 0

  ; mov eax, SELECTOR_CODE_ROOT
  ; mov ds, ax                  ; segmento de datos

  ; call screen_inicializar

  ; Cargamos la IDT
  lidt [IDT_DESC]

  mov ecx, 0
  div ecx

  ; Imprimir mensaje de bienvenida

  ; Inicializar el juego

  ; Inicializar pantalla

  ; Inicializar el manejador de memoria

  ; Inicializar el directorio de paginas

  ; Cargar directorio de paginas

  ; Habilitar paginacion

  ; Inicializar tss

  ; Inicializar tss de la tarea Idle

  ; Inicializar el scheduler

  ; Inicializar la IDT

  ; Cargar IDT

  ; Configurar controlador de interrupciones

  ; Cargar tarea inicial

  ; Habilitar interrupciones

  ; Saltar a la primera tarea: Idle

  ; Ciclar infinitamente (por si algo sale mal...)
  mov eax, 0xFFFF
  mov ebx, 0xFFFF
  mov ecx, 0xFFFF
  mov edx, 0xFFFF
  jmp $
  jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
