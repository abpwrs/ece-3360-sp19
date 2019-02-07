;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Assembly language file for Lab 2 in ECE:3360 (Embedded Systems)
; Spring 2018, The University of Iowa.
;
; Desc:
;
; B. Mitchinson, A. Powers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; 
.include "tn45def.inc"
.cseg
.org 0

.EQU SER_IN = 0
.EQU SRCK = 1
.EQU RCK = 2

; ATiny - Register Relations
; PB0 = SER IN
; PB1 = SRCK
; PB2 = RCK

; Configure PB0, PB1, and PB2 as output pins.
    sbi DDRB,0;
    sbi DDRB,1
    sbi DDRB,2

;
  main:
    sbi PORTB,SER_IN
	sbi PORTB,SRCK
	;cbi PORTB,SRCK
	sbi PORTB,RCK
	rcall delay_long
	cbi PORTB,SER_IN
	rcall delay_long
	rjmp main
	
    
   delay_long:
      ldi   r23,11      ; r23 <-- Counter for outer loop
  d1: ldi   r24,226     ; r24 <-- Counter for level 2 loop 
  d2: ldi   r25,249     ; r25 <-- Counter for inner loop
  d3: dec   r25
      nop               ; no operation 
      brne  d3 
      dec   r24
      brne  d2
      dec   r23
      brne  d1
      ret

.exit