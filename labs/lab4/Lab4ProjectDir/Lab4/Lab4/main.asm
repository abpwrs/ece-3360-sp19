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

//////////////////////////////////////////////////////////////////////

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
.equ upper_cycle_limit = 254
.equ lower_cycle_limit = 54
.equ half_duty_cycle = 154
.def duty_reg = r18
ldi duty_reg, half_duty_cycle
//
.equ BOTH_ON = 0x00
.equ A_ON = 0x01
.equ B_ON = 0x02
.equ BOTH_OFF = 0x03
// 0 - both on   0b00000000
// 1 - clockwise  0b00000001
// 2 - counter    0b00000010
// 3 - both off    0b00000011
// A is bit position 1
// B is bit position 0
////////////////////////////////////////////////////////////////////////////////


// configure PWM
//////////////////////////////////////////////////////////////////////
push r30
ldi r30, 0x23 // 0 0 1 0 0 0 1 1 (Compare to non invert + mode to 7)
out TCCR0A, r30
ldi r30, 0x09 // 0 0 0 0 1 0 0 1 (mode to 7 + prescale of 1)
out TCCR0B, r30
pop r30
// OCR0B is the pwm active reg 

// configure delay registers
//////////////////////////////////////////////////////////////////////
.def tmp1 = r21
.def tmp2 = r22
//////////////////////////////////////////////////////////////////////


// LCDstr
//////////////////////////////////////////////////////////////////////
LCDstr: .db 0x33, 0x32, 0x28, 0x01, 0x0c, 0x06
//////////////////////////////////////////////////////////////////////



 /* steps from notes for initializing LCD
 switch to command mode using RS to 0 // instruction is 0 and data input is 1
 pull read write low // o is write and 1 is read

 - Wait for 100 ms 
 – Set the device to 8-bit mode 
 – Wait 5 ms 
 – Set the device to 8-bit mode 
 – Wait at least 	
 – Set the device to 8-bit mode 
 – Wait at least 	
 – Set device to 4-bit mode 
 – Wait at least 5ms 
 – Complete additional device configuration*/




out OCR0B, duty_reg ; Set PWM flip point at 100 

; Replace with your application code




rcall lcd_init
//rcall msg1

main:

/*	rcall lcd_init
	rcall msg1
	rcall read_rpg
	rcall which_direction
	out OCR0B, duty_reg
	rcall delay*/
	rcall lcd_init
	nop

	nop
    rjmp main

msg1: 
	.db "DC = ",0x00 
	ldi r30,LOW(2*msg1)    ; Load Z register low 
	ldi r31,HIGH(2*msg1)   ; Load Z register high
	rcall displayCString

displayCString: 
	lpm r0,Z+               ; <-- first byte 
	tst r0                  ; Reached end of message ? 
	breq done               ; Yes => quit 
	swap  r0                ; Upper nibble in place 
	out   PORTC,r0          ; Send upper nibble out 
	rcall lcd_strobe         ; Latch nibble 
	swap  r0                ; Lower nibble in place 
	out   PORTC,r0          ; Send lower nibble out 
	rcall lcd_strobe         ; Latch nibble 
	rjmp displayCstring 
done: 
	ret

load_command_nibble:
	rcall delay_10_ms
	sbi PORTB, E
	nop
	out PORTC, data_reg
	nop
	cbi PORTB, E
	rcall lcd_strobe
	ret
	

lcd_strobe:
	cbi PORTB, E
	rcall delay_200_us
	sbi PORTB, E
	rcall delay_200_us
	cbi PORTB, E
	ret
	
lcd_init:
	rcall delay_100_ms
	rcall set_to_8_bit_mode
	rcall delay_10_ms
	rcall set_to_8_bit_mode
	rcall delay_200_us
	rcall set_to_8_bit_mode
	rcall delay_200_us
	rcall set_to_4_bit_mode
	rcall delay_10_ms

	ldi data_reg, 0x02
	rcall load_command_nibble

	ldi data_reg, 0x08
	rcall load_command_nibble

	ldi data_reg, 0x00
	rcall load_command_nibble

	ldi data_reg, 0x01
	rcall load_command_nibble

	ldi data_reg, 0x00
	rcall load_command_nibble

	ldi data_reg, 0x0c
	rcall load_command_nibble

	ldi data_reg, 0x00
	rcall load_command_nibble

	ldi data_reg, 0x06
	rcall load_command_nibble

	ret

set_to_8_bit_mode:
	rcall delay_10_ms
	cbi PORTB, RS
	nop
	rcall lcd_strobe
	nop
	sbi PORTB, E
	nop
	ldi data_reg, 0x03
	nop
	out PORTC, data_reg
	nop
	cbi PORTB, E
	nop
	rcall lcd_strobe

	ret

set_to_4_bit_mode:
	rcall delay_10_ms
	cbi PORTB, RS
	nop
	rcall lcd_strobe
	nop
	sbi PORTB, E
	nop
	ldi data_reg, 0x02
	nop
	out PORTC, data_reg
	nop
	cbi PORTB, E
	nop
	rcall lcd_strobe

	ret

read_rpg:
    nop
    push r28
    push r29
    ldi r28, 0x01e
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


// clockwise
clockwise:
    inc duty_reg
    cpi duty_reg, upper_cycle_limit
    brsh recover_upper
    rjmp end_cwise
    recover_upper:
    ldi duty_reg, upper_cycle_limit
    end_cwise:
    ret

// counter_clockwise
counter_clockwise:
    dec duty_reg
    cpi duty_reg, lower_cycle_limit
    brsh end_ccwise
    ldi duty_reg, lower_cycle_limit
    end_ccwise:
    ret

delay:
    ; Stop timer 0
    in tmp1, TCCR0B
    ldi tmp2, 0x00
    out TCCR0B, tmp2

    ; Clear over flow flag
    in tmp2, TIFR0
    sbr tmp2, 1<<TOV0
    out TIFR0, tmp2

    ; Start timer with new initial count
	push r30
	ldi r30, 0x37
    out TCNT0, r30
    out TCCR0B, tmp1
	pop r30

; wait
wait:
    in tmp2, TIFR0
    sbrs tmp2, TOV0
    rjmp wait

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

//////////////////////////////////////////////////////////////////////
.exit

