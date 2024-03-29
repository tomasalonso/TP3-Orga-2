  ; ** por compatibilidad se omiten tildes **
  ; ==============================================================================
  ; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
  ; ==============================================================================

%include "imprimir.mac"

extern sched_inicializar
extern game_inicializar
extern tss_inicializar
extern screen_inicializar
extern idt_inicializar
extern mmu_inicializar_dir_kernel
extern mmu_inicializar
extern resetear_pic
extern habilitar_pic
extern GDT_DESC                 ; para inicializar la GDT
extern IDT_DESC                 ; para inicializar la IDT
extern pagina_tss
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

  ; Ejercicio 1.b
  ; Cargar la GDT
  lgdt [GDT_DESC]

  ; Setear el bit PE del registro CR0
  mov eax, CR0
  or eax, 1                   ; es el bit 0
  mov CR0, eax

  ; Saltar a modo protegido
  jmp (8 << 3):mp           ; carga el cs con 8:000

BITS 32
mp:
  ; Establecer selectores de segmentos
  xor eax, eax
%define GDT_IDX_ROOT_DATA 9 << 3
  mov eax, GDT_IDX_ROOT_DATA
  mov ds, ax                  ; segmento de datos
  mov ss, ax                  ; segmento de pila, apunta igual que el segmento de datos
  mov es, ax
  mov gs, ax
  mov fs, ax

  ; Establecer la base de la pila
  mov esp, 0x27000
  mov ebp, 0x27000

  imprimir_texto_mp iniciando_mp_msg, iniciando_mp_len, 0x07, 2, 0

  ; Ejercicio 1.d
; %define SELECTOR_VIDEO 12 << 3
;   mov eax, SELECTOR_VIDEO
;   mov fs, ax                  ; segmento de datos, apunta a segmento de video

; ; seteamos la pantalla a gris
; %define pos_ecx ecx
; %define i_eax eax
; %define j_ebx ebx
;   xor pos_ecx, pos_ecx           ; pos := 0
;   xor i_eax, i_eax               ; i := 0
; ciclo_i:

;   xor j_ebx, j_ebx               ; j := 0
; ciclo_j:
;   ; el primero es de carácter y el segundo de color...
;   mov byte [fs:pos_ecx], 0x00
;   mov byte [fs:pos_ecx+1], (0x7 << 4)
;   lea pos_ecx, [pos_ecx + 2]

;   inc j_ebx
;   cmp j_ebx, 80
;   jne ciclo_j

;   inc i_eax
;   cmp i_eax, 50
;   jne ciclo_i

;   mov eax, SELECTOR_CODE_ROOT
;   mov fs, ax                      ; segmento de datos

  ; Ejercicio 2.a
  ; Cargar IDT
  call idt_inicializar            ; Inicializamos la idt con las funciones _isr
  ; Inicializar la IDT
  lidt [IDT_DESC]                 ; Cargamos el pseudo-descriptor de la IDT

  ; Ejercicio 2.b
; %define aux_ecx ecx
;   mov aux_ecx, 0                  ; aux := 0
;   div aux_ecx                     ; división por 0
;   int 3                           ; break
;   add eax, 0x0
;   sub eax, 0xFFFFFFFF
;   into                            ; overflow
;   int 13
;   mov aux_ecx, [ds:0xFFFFFFF0]    ; se va del limite
;   int 8                           ; doble falta, 15 minutos afuera
; %assign i 0
; %rep    14
;   int i
; %assign i i+1
; %endrep


  ; Inicializar el directorio de paginas
  ; Ejercicio 3.b
  call mmu_inicializar_dir_kernel

  ; Cargar directorio de paginas
  ; Ejercicio 3.c
%define page_directory 0x27000
  mov eax, page_directory
  mov cr3, eax                  ; cargamos dirección de page directory

  ; Habilitar paginacion
  mov eax, cr0
  or eax, 0x80000000            ; bit paginación activado
  mov cr0, eax                  ; paginación habilitada

  ; Inicializar el manejador de memoria
  call mmu_inicializar

  ; Ejercicio 5.a
  ; Configurar controlador de interrupciones
  call resetear_pic
  call habilitar_pic

  ; Prueba de interrupción 0x46
  ; int 0x46

  ; Inicializar tss de la tarea inicial
  ; Inicializar tss de la tarea Idle
  call tss_inicializar

  ; Imprimir mensaje de bienvenida
  ; Inicializar el juego
  call game_inicializar

  ; Inicializar el scheduler
  call sched_inicializar

  ; Inicializar pantalla
  call screen_inicializar

  ; Habilitar interrupciones
  sti                           ; Activamos las interrupciones enmascarables

  ; Cargar tarea inicial
%define GDT_TSS_INITIAL 12
  mov ax, GDT_TSS_INITIAL << 3
  ltr ax

  ; ; Asaltar al pirata (prueba)
  ; xchg bx, bx
  ; jmp (14 << 3):0

%define GDT_TSS_IDLE 13
  ; Saltar a la primera tarea: Idle
  jmp (GDT_TSS_IDLE << 3):0

  ; Ciclar infinitamente (por si algo sale mal...)
  mov eax, 0xFFFF
  mov ebx, 0xFFFF
  mov ecx, 0xFFFF
  mov edx, 0xFFFF
  jmp $
  jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
