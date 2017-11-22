; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual

; Ejercicio 5.b
extern game_tick
; Ejercicio 5.c
extern game_atender_teclado

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

; Vamo' a usar la macro, pero la vamo' a usar, como ¡YO QUIERA!
%macro ISR 2
global _isr%1

  msg%1 db %2
  msg%1_len equ    $ - msg%1

_isr%1:
  mov eax, %1
  imprimir_texto_mp msg%1, msg%1_len, 0x7, 0, 0
  iret
%endmacro

;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler

;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
  ; Ejercicio 2.a
  ISR 0, 'Dividiste por 0, animal!'
  ISR 1, 'Excepcion para debug'
  ISR 2, 'Interrupcion de E/S (NMI)'
  ISR 3, 'Break! accion?'
  ISR 4, 'A la mierda, overflow!'
  ISR 5, 'Te fuiste de rango'
  ISR 6, 'Esa instruccion la pifio'
  ISR 7, 'No se pudo usar el dispositivo para hacer la cuenta'
  ISR 8, 'Hasta la vista, baby'
  ISR 9, 'Esta es del año del pedo. Fallo el coprocesador matematico'
  ISR 10, 'Problemas con la TSS'
  ISR 11, 'No esta el segmento'
  ISR 12, 'Se acabo o no hay pila'
  ISR 13, 'Problema con el segmento'
  ISR 14, 'Problema con la paginacion'
  ; ISR 15, 'Reservada'
  ISR 16, 'Problema con la FPU'
  ISR 17, 'Memoria desalineada'
  ISR 18, 'Problema con el bus. Tira la compu y comprate otra'
  ISR 19, 'Problema con SIMD'

; Ejercicio 5.a
;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32
_isr32:
  pushad
  call fin_intr_pic1
  ; Ejercicio 5.b
  call game_tick
  popad
  iret

;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33
_isr33:
  pushad
  call fin_intr_pic1
  xor eax, eax
  in al, 0x60                   ; leemos la tecla presionada
  push eax
  call game_atender_teclado
  pop eax

  ; Ejercicio 5.a
  ; para pruebas, chequea si es 'a'
  ; cmp al, 0x1E
  ; jne .pasar
  ; xchg bx, bx
; .pasar:

  popad
  iret


;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
#define MOVERSE 0x1
#define CAVAR 0x2
#define POSICION 0x3

global _isr70 ; 0x46
_isr70:
  pushad
  cmp eax, MOVERSE
  jne .cavar
  push ecx
  call game_syscall_pirata_mover
  jmp .fin
.cavar:
  cmp eax, CAVAR
  call game_syscall_cavar
  jne .posicion

.posicion:
  cmp eax, POSICION
  call game_syscall_pirata_posicion

.fin:
  popad
  ; Ejercicio 5.d
  ; mov eax, 0x42
  iret
