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
;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;

;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;


