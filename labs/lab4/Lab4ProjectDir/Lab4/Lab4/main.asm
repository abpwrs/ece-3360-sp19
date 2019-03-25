//////////////////////////////////////////////////////////////////////
// Assembly language file for Lab 4 in ECE:3360 (Embedded Systems)
// Spring 2019, The University of Iowa.
//
// Desc: Lab 4 Rotary Pulse Generator Controlled Fan With LCD Display
//
// Authors: B. Mitchinson, A. Powers
//////////////////////////////////////////////////////////////////////

// definitions and preprocessor directives
//=============================================================================

// .inc include files
//////////////////////////////////////////////////////////////////////
.include "m88PAdef.inc"

// set port numbers
//////////////////////////////////////////////////////////////////////
.equ led_port = 5
.equ pwm_port = 5

// LCD config
//////////////////////////////////////////////////////////////////////
.equ RS = 5
.equ E = 3
.equ D4 = 0
.equ D5 = 1
.equ D6 = 2
.equ D7 = 3
.def data_reg = r19

// set DDRB/C
//////////////////////////////////////////////////////////////////////
sbi DDRC, led_port
sbi DDRD, pwm_port
sbi DDRC, D4
sbi DDRC, D5
sbi DDRC, D6
sbi DDRC, D7
sbi DDRB, RS
sbi DDRB, E

// variables for current and previous state of the rpg
////////////////////////////////////////////////////////////////////////////////
.def current_state = r16
.def previous_state = r17
.def duty_reg = r18
.equ upper_cycle_limit = 201
.equ lower_cycle_limit = 1
.equ half_duty_cycle = 60
ldi duty_reg, half_duty_cycle

.equ BOTH_ON = 0x03
.equ A_ON = 0x01
.equ B_ON = 0x02
.equ BOTH_OFF = 0x00
// 0 - both on   0b00000000
// 1 - clockwise  0b00000001
// 2 - counter    0b00000010
// 3 - both off    0b00000011
// A is bit position 1
// B is bit position 0
////////////////////////////////////////////////////////////////////////////////

// Registers for displayDC
//////////////////////////////////////////////////////////////////////


// configure PWM
//////////////////////////////////////////////////////////////////////
push r29
ldi r29, 0x23 // 0 0 1 0 0 0 1 1 (Compare to non invert + mode to 7)
out TCCR0A, r29
ldi r29, 0x09 // 0 0 0 0 1 0 0 1 (mode to 7 + prescale of 1)
out TCCR0B, r29

ldi r29, 201
out OCR0A, r29

out OCR0B, duty_reg ; Set PWM flip point, OCR0B is the pwm active reg
pop r29

// Tables
//////////////////////////////////////////////////////////////////////
msg_dc: .db "DC =      %", 0x00
msg_0: .db "  0.0", 0x00
msg_100: .db "100.0", 0x00
msg_a: .db "Mode A:", 0x00
msg_b: .db "Mode B:", 0x00

.dseg
	active: .BYTE 5

.cseg

push data_reg
rcall lcd_init
pop data_reg

push r25
push r26
ldi r25,low(755)
ldi r26,high(755)
rcall displayDC
rcall displayDString
pop r26
pop r25

; 30 and 31 are always Z
/*ldi R30, LOW(2*active)
ldi R31, HIGH(2*active)
sbi PORTB, RS
rcall displayCString*/

/*ldi R30, LOW(2*msg_dc)
ldi R31, HIGH(2*msg_dc)
sbi PORTB, RS
rcall displayCString

cbi PORTB, 5;
ldi data_reg, 0x0C
out PORTC, data_reg
rcall lcd_strobe
rcall delay_200_us
ldi data_reg, 0x00
out PORTC, data_reg
rcall lcd_strobe
rcall delay_200_us

sbi PORTB, 5
ldi r30, LOW(2*msg_a)
ldi r31, HIGH(2*msg_a)
rcall displayCString*/

main:
	// RPG Sub-Methods 
	rcall read_rpg // Uses 16, 17
	rcall which_direction // 16, 17, 18
	rcall delay

	// Fan Signal to display mode result: 19 - 29 available
	// push regs
	// call stuff
	// pop regs

	// Fill in duty cycle %:  19 - 29 available
	// push registers
	push r14
	push r15
	push r16
	push r17
	push r18
	push r19
	push r20

	// TODO: prep 25 and 26 for displayDC
	// Call displayDC
	// Call displayDstring

	// pop registers
	pop r20
	pop r19
	pop r18
	pop r17
	pop r16
	pop r15
	pop r14


	rjmp main

displayCString:             ; Prints whatever is in Z
	sbi PORTB, RS
	lpm r0,Z+               ; <-- first byte 
	tst r0                  ; Reached end of message ? 
	breq doneC               ; Yes => quit 
	swap  r0                ; Upper nibble in place 
	out   PORTC,r0          ; Send upper nibble out 
	rcall lcd_strobe         ; Latch nibble 	
	rcall delay_10_ms
	swap  r0                ; Lower nibble in place 
	out   PORTC,r0          ; Send lower nibble out 
	rcall lcd_strobe         ; Latch nibble 
	rcall delay_10_ms
	rjmp displayCstring 
doneC: 
	ret

displayDC:             ; Converts 3 digits in r25 r26 to active
	
	push r21

	mov dd16uL,r25     ; LSB of number to display
	mov dd16uH,r26     ; MSB of number to display  
	ldi dv16uL,low(10) 
	ldi dv16uH,high(10)

	; Store terminating for the string. 
	ldi r20,0x00       ; Terminating NULL     
	sts active+4,r20     ; Store in RAM

	; Divide the number by 10 and format remainder. 
	rcall div16u       ; Result: r17:r16, rem: r15:r14 
	ldi r20,0x30 
	add r14,r20      ; Convert to ASCII 
	sts active+3,r14     ; Store in RAM
	
	; Generate decimal point. 
	ldi r20,0x2e       ; ASCII code for . 
	sts active+2,r20     ; Store in RAM

	mov dd16uL, r16 
	mov dd16uH, r17
	rcall div16u

	ldi r20,0x30
	add r14, r20
	sts active+1,r14

	ldi r20,0x30
	add r16, r20
	sts active+0,r16

	pop r21
	ret

displayDString:             ; Prints whatever is in Z
	sbi PORTB, RS
	ldi r30, LOW(active)
	ldi r31, HIGH(active)
progressDString:
	ld r0,Z+               ; <-- first byte 
	tst r0                  ; Reached end of message ? 
	breq doneD               ; Yes => quit 
	swap  r0                ; Upper nibble in place 
	out   PORTC,r0          ; Send upper nibble out 
	rcall lcd_strobe         ; Latch nibble 	
	;rcall delay_10_ms
	swap  r0                ; Lower nibble in place 
	out   PORTC,r0          ; Send lower nibble out 
	rcall lcd_strobe         ; Latch nibble 
	;rcall delay_10_ms
	rjmp progressDString 
doneD: 
	ret

lcd_strobe:
	cbi PORTB, E
	rcall delay_200_us
	sbi PORTB, E
	rcall delay_200_us
	cbi PORTB, E
	ret

lcd_init:
	cbi PORTB, RS
	rcall delay_10_ms
	rcall set_to_8_bit_mode
	rcall set_to_8_bit_mode
	rcall set_to_8_bit_mode
	rcall set_to_4_bit_mode
	rcall set_to_4_bit_mode

	ldi data_reg, 0x08 ; Two rows 5x7 characters
	out PORTC, data_reg
	rcall lcd_strobe
	rcall delay_200_us

	ldi data_reg, 0x00 ; clear display
	out PORTC, data_reg
	rcall lcd_strobe
	rcall delay_200_us

	ldi data_reg, 0x01 ; also clear display
	out PORTC, data_reg
	rcall lcd_strobe
	rcall delay_10_ms

	ldi data_reg, 0x00 ; display on, underline + blink off
	out PORTC, data_reg
	rcall lcd_strobe
	rcall delay_200_us

	ldi data_reg, 0x0c ; also display on, underline + blink off 
	out PORTC, data_reg
	rcall lcd_strobe
	rcall delay_200_us

	ldi data_reg, 0x00 ; display shift off, address increment mode
	out PORTC, data_reg
	rcall lcd_strobe
	rcall delay_200_us

	ldi data_reg, 0x06 ; also display shift off, address increment mode
	out PORTC, data_reg
	rcall lcd_strobe
	rcall delay_200_us
	
	ret

set_to_8_bit_mode:
	ldi data_reg, 0x03
	nop
	out PORTC, data_reg
	rcall lcd_strobe
	rcall delay_200_us
	nop
	ret

set_to_4_bit_mode:
	ldi data_reg, 0x02
	nop
	out PORTC, data_reg
	rcall lcd_strobe
	rcall delay_200_us
	nop
	ret

// Delaying for PWM
//////////////////////////////////////////////////////////////////////
delay:
	push r21
	push r20
    // Stop timer 0
    in r20, TCCR0B //
    ldi r21, 0x00
    out TCCR0B, r21

    // Clear over flow flag
    in r21, TIFR0
    sbr r21, 1<<TOV0
    out TIFR0, r21

    // Start timer with new initial count
	push r29
	ldi r29, 0x00
    out TCNT0, r29 // starting point of timer
    out TCCR0B, r20
	pop r29
	pop r20
	
wait: // check the timer overflow bit
    in r21, TIFR0
    sbrs r21, TOV0
    rjmp wait
	pop r21
    ret

// 100ms, 10ms, and 200us delays for LCD initialization
//////////////////////////////////////////////////////////////////////
delay_100_ms:
	  push r23
	  push r24
	  push r25
	  nop
	  ldi r23, 20      ; r23 <-- Counter for outer loop
  d11: ldi r24, 44     ; r24 <-- Counter for level 2 loop
  d12: ldi r25, 227     ; r25 <-- Counter for inner loop
  d13: dec r25
      nop               ; no operation
      brne d13
      dec r24
      brne d12
      dec r23
      brne d11
	  nop
	  pop r25
	  pop r24
	  pop r23
      ret

delay_10_ms:
	  push r23
	  push r24
	  push r25
	  nop
	  ldi r23, 2      ; r23 <-- Counter for outer loop
  d21: ldi r24, 44     ; r24 <-- Counter for level 2 loop
  d22: ldi r25, 227     ; r25 <-- Counter for inner loop
  d23: dec r25
      nop               ; no operation
      brne d23
      dec r24
      brne d22
      dec r23
      brne d21
	  nop
	  pop r25
	  pop r24
	  pop r23
      ret

delay_200_us:
	  push r23
	  push r24
	  push r25
	  nop
	  ldi r23, 1       ; r23 <-- Counter for outer loop
  d31: ldi r24, 5       ; r24 <-- Counter for level 2 loop
  d32: ldi r25, 100     ; r25 <-- Counter for inner loop
  d33: dec r25
      nop               ; no operation
      brne d33
      dec r24
      brne d32
      dec r23
      brne d31
	  nop
	  pop r25
	  pop r24
	  pop r23
      ret
// RPG sub-routines
//////////////////////////////////////////////////////////////////////
read_rpg:
    nop
    push r28
    push r29
    ldi r28, 0x01
    ldi r29, 0x02

    mov previous_state, current_state
    ldi current_state, 0x00
    sbis PIND, 0
    add current_state, r29 ; run if a is high
    sbis PIND, 1
    add current_state, r28 ; run if b is high
    pop r29
    pop r28
    ret

which_direction:
    nop
    cpi previous_state, BOTH_ON
    breq which_end

    // if current state low
    cpi current_state, BOTH_OFF
    breq current_low
    rjmp which_end

    current_low:
    cpi previous_state, A_ON
    breq counter_clockwise
    cpi previous_state, B_ON
    breq clockwise
    rjmp which_end

    which_end:
    ret

clockwise:
    inc duty_reg
    cpi duty_reg, upper_cycle_limit
    brsh recover_upper
    rjmp end_cwise
    recover_upper:
    ldi duty_reg, upper_cycle_limit
    end_cwise:
	out OCR0B, duty_reg
    ret

counter_clockwise:
    dec duty_reg
    cpi duty_reg, lower_cycle_limit
    brsh end_ccwise
    ldi duty_reg, lower_cycle_limit
    end_ccwise:
	out OCR0B, duty_reg
    ret
//////////////////////////////////////////////////////////////////////

;***************************************************************************
;*
;* "div16u" - 16/16 Bit Unsigned Division
;*
;* This subroutine divides the two 16-bit numbers 
;* "dd8uH:dd8uL" (dividend) and "dv16uH:dv16uL" (divisor). 
;* The result is placed in "dres16uH:dres16uL" and the remainder in
;* "drem16uH:drem16uL".
;*  
;* Number of words	:19
;* Number of cycles	:235/251 (Min/Max)
;* Low registers used	:2 (drem16uL,drem16uH)
;* High registers used  :5 (dres16uL/dd16uL,dres16uH/dd16uH,dv16uL,dv16uH,
;*			    dcnt16u)
;*
;***************************************************************************

;***** Subroutine Register Variables

.def	drem16uL=r14
.def	drem16uH=r15
.def	dres16uL=r16
.def	dres16uH=r17
.def	dd16uL	=r16
.def	dd16uH	=r17
.def	dv16uL	=r18
.def	dv16uH	=r19
.def	dcnt16u	=r20

;***** Code

div16u:	clr	drem16uL	;clear remainder Low byte
	sub	drem16uH,drem16uH;clear remainder High byte and carry
	ldi	dcnt16u,17	;init loop counter
d16u_1:	rol	dd16uL		;shift left dividend
	rol	dd16uH
	dec	dcnt16u		;decrement counter
	brne	d16u_2		;if done
	ret			;    return
d16u_2:	rol	drem16uL	;shift dividend into remainder
	rol	drem16uH
	sub	drem16uL,dv16uL	;remainder = remainder - divisor
	sbc	drem16uH,dv16uH	;
	brcc	d16u_3		;if result negative
	add	drem16uL,dv16uL	;    restore remainder
	adc	drem16uH,dv16uH
	clc			;    clear carry to be shifted into result
	rjmp	d16u_1		;else
d16u_3:	sec			;    set carry to be shifted into result
	rjmp	d16u_1

.exit
