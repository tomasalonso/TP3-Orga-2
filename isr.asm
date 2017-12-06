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

;; Excepciones
extern game_atender_excepcion


; Ejercicio 5.b
extern game_tick
; Ejercicio 5.c
extern game_atender_teclado

; Ejercicio 6.g
extern game_syscall_manejar

; Ejercicio 7.c
extern sched_tick

; Para terminar
extern tss_finalizar
extern sched_finalizar
extern game_calcular_fin

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;
%define GDT_TSS_IDLE 13
%define FIN 1

%define EFLAGS 2*4
%define EIP 0*4
%define ESP 3*4
%define SS 4*4
%define CS 1*4

; Vamo' a usar la macro, pero la vamo' a usar, como ¡YO QUIERA!
%macro ISR 2+
%if %0 = 3
%define ERROR_CODE 1*4
%else
%define ERROR_CODE 0
%endif

global _isr%1

  msg%1 db %2, 0
  msg%1_len equ    $ - msg%1

_isr%1:
  pushad
  push esp

  ; mov eax, %1
  ; imprimir_texto_mp msg%1, msg%1_len, 0x7, 0, 0

  mov eax, [esp+9*4+EFLAGS+ERROR_CODE]
  push eax

  mov eax, [esp+10*4+EIP+ERROR_CODE]
  push eax

  mov eax, [esp+11*4+ESP+ERROR_CODE]
  push eax

  xor eax, eax
  mov ax, gs
  push eax
  mov ax, fs
  push eax
  mov ax, es
  push eax
  mov ax, ds
  push eax
  mov ax, [esp+16*4+SS+ERROR_CODE]
  push eax
  mov ax, [esp+17*4+CS+ERROR_CODE]
  push eax

  push msg%1

  call game_atender_excepcion
  add esp, 18*4

  jmp (GDT_TSS_IDLE << 3):0
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
  ISR 8, 'Hasta la vista, baby', 'tiene error code'
  ISR 9, 'Esta es del año del pedo. Fallo el coprocesador matematico'
  ISR 10, 'Problemas con la TSS', 'tiene error code'
  ISR 11, 'No esta el segmento', 'tiene error code'
  ISR 12, 'Se acabo o no hay pila', 'tiene error code'
  ISR 13, 'Problema con el segmento', 'tiene error code'
  ISR 14, 'Problema con la paginacion', 'tiene error code'
  ; ISR 15, 'Reservada'
  ISR 16, 'Problema con la FPU'
  ISR 17, 'Memoria desalineada', 'tiene error code'
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

  call game_calcular_fin
  cmp eax, FIN                  ; terminar?
  jne .sched

  mov dword [esp+(3+8)*4], 0x00000002 ; desactivamos las interrupciones (en caso de ya estar en la idle)
  call tss_finalizar            ; desactiva int de la tss de la tarea idle
  call sched_finalizar          ; devuelve la idle
  jmp .switch

.sched:
  ; ; Ejercicio 5.b
  ; call game_tick
  call sched_tick
.switch:
  shl ax, 3                     ; agregamos CPL al índice

  str cx
  cmp ax, cx                    ; es la misma tarea?
  je .fin

.salto:
  mov [sched_tarea_selector], ax
  jmp far [sched_tarea_offset]

.fin:
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
global _isr70 ; 0x46
_isr70:
  pushad

  push ecx
  push eax
  call game_syscall_manejar
  pop ecx                       ; reestablecer eax
  add esp, 4                    ; restablezco la pila

  cmp eax, -1                   ; hubo error (el pirata exploto)
  je .saltar
.cavar:                         ; agrega la posicion a manopla
  cmp ecx, 0x3
  jne .saltar
  mov [esp+7*4], eax

.saltar:
  jmp (GDT_TSS_IDLE << 3):0

  popad
  ; Ejercicio 5.d
  ; mov eax, 0x42
  iret
