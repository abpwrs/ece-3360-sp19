;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Assembly language file for Lab 2 in ECE:3360 (Embedded Systems)
; Spring 2018, The University of Iowa.
;
; Desc:
;
; B. Mitchinson, A. Powers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Program Description:
; main.asm runs the control logic for a buttone controlled counter
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.include "tn45def.inc"
.cseg
.org 0

.EQU SER_IN = 0
.EQU SRCK = 1
.EQU RCK = 2
.EQU MAX_VALUE = 15


; ATiny - Register Relations
; PB0 = SER IN
; PB1 = SRCK
; PB2 = RCK

; Configure PB0, PB1, and PB2 as output pins.
    sbi DDRB,0
    sbi DDRB,1
    sbi DDRB,2
	sbi DDRB,3
	sbi DDRB,4
;
  main:
;	sbi PORTB, 3
;	sbi PORTB, 4
;    sbi PORTB,SER_IN
;	rcall delay_long
;
;	sbi PORTB,SRCK
;	rcall delay_long
;
;	cbi PORTB,SRCK
;	rcall delay_long
;
;	sbi PORTB,RCK
;	rcall delay_long
;
;	cbi PORTB, RCK
;	rcall delay_long
;
;	cbi PORTB,SER_IN
;	rcall delay_long

;	rcall shift_one_on
;	rcall shift_one_on
;	rcall reset_disp
;	rcall delay_long
;	rcall shift_one_off
;	rcall shift_one_off
;	rcall reset_disp
;	rcall delay_long

	rcall disp_zero
	rcall delay_long

	rcall disp_one
	rcall delay_long

	rcall disp_two
	rcall delay_long

	rcall disp_three
	rcall delay_long

	rcall disp_four
	rcall delay_long

	rcall disp_five
	rcall delay_long

	rcall disp_six
	rcall delay_long

	rcall disp_seven
	rcall delay_long

	rcall disp_eight
	rcall delay_long

	rcall disp_nine
	rcall delay_long

	rcall disp_a
	rcall delay_long

	rcall disp_b
	rcall delay_long

	rcall disp_c
	rcall delay_long

	rcall disp_d
	rcall delay_long

	rcall disp_e
	rcall delay_long

	rcall disp_f
	rcall delay_long

	rjmp main
	
shift_one_on:
	sbi PORTB, SER_IN
	nop
	sbi PORTB, SRCK
	nop
	cbi PORTB, SRCK
	ret

shift_one_off:
	cbi PORTB, SER_IN
	nop
	sbi PORTB, SRCK
	nop
	cbi PORTB, SRCK
	ret

reset_disp:
	sbi PORTB, RCK
	nop
	cbi PORTB, RCK
	ret
    
    
delay_long:
      ldi   r23,20      ; r23 <-- Counter for outer loop
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

; 0-1-1-1--0-1-1-1
; 0x77
disp_zero:
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall reset_disp
	ret


; 0-0-0-0--0-1-1-0
; 0x06
disp_one:
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall reset_disp
	ret

; 1-0-1-1--0-0-1-1
; 0xB3
disp_two:
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall reset_disp
	ret

;1-0-0-1--0-1-1-1
; 0x97
disp_three:
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall reset_disp
	ret

; 1-1-0-0--0-1-1-0
; 0xC6
disp_four:
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall reset_disp
	ret

; 1-1-0-1--0-1-0-1
; 0xD5
disp_five:
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall reset_disp
	ret

; 1-1-1-1--0-1-0-1
; 0xF5
disp_six:
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall reset_disp
	ret

; 0-0-0-0--0-1-1-1
; 0x07
disp_seven:
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall reset_disp
	ret

; 1-1-1-1--0-1-1-1
; 0xF7
disp_eight:
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall reset_disp
	ret

; 1-1-0-1--0-1-1-1
; 0xD7
disp_nine:
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall reset_disp
	ret

; 1-1-1-0--0-1-1-1
; 0xE7
disp_a:
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall reset_disp
	ret

; 1-1-1-1--0-1-0-0
; 0xF4
disp_b:
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_off
	rcall reset_disp
	ret

; 0-1-1-1--0-0-0-1
; 0x71
disp_c:
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_on
	rcall reset_disp
	ret

; 1-0-1-1--0-1-1-0
; 0xB6
disp_d:
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall reset_disp
	ret

; 1-1-1-1--0-0-0-1
; 0xF1
disp_e:
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_on
	rcall reset_disp
	ret

; 1-1-1-0--0-0-0-1
; 0xE1
disp_f:
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_on
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_off
	rcall shift_one_on
	rcall reset_disp
	ret

.exit
