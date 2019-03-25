.include "m88PAdef.inc"

// LCD config
//////////////////////////////////////////////////////////////////////
.equ RS = 5
.equ E = 3
.equ D4 = 0
.equ D5 = 1
.equ D6 = 2
.equ D7 = 3
.def data_reg = r19

// set port numbers
//////////////////////////////////////////////////////////////////////
.equ led_port = 5
.equ pwm_port = 5

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

// Tables
//////////////////////////////////////////////////////////////////////
LCDInit: .db 0x33, 0x32, 0x28, 0x01, 0x0c, 0x06
msg_dc: .db "DC =      %", 0x00
msg_a: .db "Mode A:", 0x00

rcall lcd_init

ldi R30, LOW(2*msg_dc)
ldi R31, HIGH(2*msg_dc)
sbi PORTB, RS
rcall displayCString 
// Only difference is ours has extra delays

cbi PORTB, 5;
ldi r19, 0x0C
out PORTC, r19
rcall lcd_strobe
rcall delay_200_us
ldi r19, 0x00
out PORTC, r19
rcall lcd_strobe
rcall delay_200_us

sbi PORTB, 5
ldi r30, LOW(2*msg_a)
ldi r31, HIGH(2*msg_a)
rcall displayCString

main:
	rjmp main

displayCString:
	lpm r0,Z+               ; <-- first byte 
	tst r0                  ; Reached end of message ? 
	breq done               ; Yes => quit 
	swap  r0                ; Upper nibble in place 
	out   PORTC,r0          ; Send upper nibble out 
	rcall lcd_strobe         ; Latch nibble 	
	rcall delay_10_ms
	swap  r0                ; Lower nibble in place 
	out   PORTC,r0          ; Send lower nibble out 
	rcall lcd_strobe         ; Latch nibble 
	rcall delay_10_ms
	rjmp displayCstring 
done: 
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

.exit
