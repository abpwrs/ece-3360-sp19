;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Assembly language file for Lab 2 in ECE:3360 (Embedded Systems)
; Spring 2018, The University of Iowa.
;
; Desc: runs the control logic for a buttone controlled counter
;
; B. Mitchinson, A. Powers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
;

.include "tn45def.inc"
.include "disp_values.inc"

.dseg
.org 0x0060 ;Tiny45, Start in SRAM
;disp_table: .db 0x77, 0x06, 0xB3, 0x97, 0xc6, 0xd5, 0xf5, 0x07, 0xf7, 0xd7, 0xe7, 0xf4, 0x71, 0xb6, 0xf1, 0xe1
disp_table: .byte 16

.cseg
.org 0x0000

; test initialize SRAM
; TODO: Can I move this out to a different file? Can disp_table be a db like the original goal?
ldi r20, 0x77
sts disp_table, r20
ldi r20, 0x06
sts disp_table+1, r20
ldi r20, 0xB3
sts disp_table+2, r20
ldi r20, 0x97
sts disp_table+3, r20
ldi r20, 0xc6
sts disp_table+4, r20
ldi r20, 0xd5
sts disp_table+5, r20
ldi r20, 0xf5
sts disp_table+6, r20
ldi r20, 0x07
sts disp_table+7, r20
ldi r20, 0xf7
sts disp_table+8, r20
ldi r20, 0xd7
sts disp_table+9, r20
ldi r20, 0xe7
sts disp_table+10, r20
ldi r20, 0xf4
sts disp_table+11, r20
ldi r20, 0x71
sts disp_table+12, r20
ldi r20, 0xb6
sts disp_table+13, r20
ldi r20, 0xf1
sts disp_table+14, r20
ldi r20, 0xe1
sts disp_table+15, r20

; 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.equ SER_IN = 0
.equ SRCK = 1
.equ RCK = 2
.equ PUSH_BUTTON = 4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.DEF DISP_REG = R16
.DEF BUTTON_TIME_REG = R18
.DEF DEC_REG = R19
.DEF Z_OFFSET = R20

; ATiny - Register Relations
; PB0 = SER_IN
; PB1 = SRCK
; PB2 = RCK
; Configure PB0, PB1, and PB2 as output pins.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sbi DDRB,0
sbi DDRB,1
sbi DDRB,2
cbi DDRB,3
cbi DDRB,4

ldi ZL, low(disp_table) ;r30
ldi ZH, high(disp_table) ;r31
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; main method -- infinite loop to keep the controller responding to input
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rcall display
main:
nop
sbis PINB, PUSH_BUTTON ; if button pushed, next line will execute
rcall button_pressed ; react to the button press
rjmp main
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
button_pressed:
ldi BUTTON_TIME_REG, 0x00 ; set initial state of counter to zero
rcall count_press
rjmp update

; count press sets certain register values associated with the length of button press
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
count_press:
rcall sample_delay
cpi BUTTON_TIME_REG, 255
breq skip
inc BUTTON_TIME_REG
skip:
sbis PINB, PUSH_BUTTON ; if button is still pressed, execute next line
rjmp count_press ; rjmp to this method
ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; TODO: Update with proper desmos calc: 
; my delay of 10000 cycles -- currently ~ 10032 cycles
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sample_delay:
         ldi   r23,20      ; r23 <-- Counter for outer loop
  my_d1: ldi   r24,24     ; r24 <-- Counter for level 2 loop 
  my_d2: ldi   r25,41      ; r25 <-- Counter for inner loop
  my_d3: dec   r25
         nop              ; no operation 
         brne  my_d3 
         dec   r24
         brne  my_d2
         dec   r23
         brne  my_d1
         ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; update the state based on the contents of BUTTON_TIME_REG
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
update:

cpi BUTTON_TIME_REG, 200
brsh reset_routine

cpi BUTTON_TIME_REG, 100
brsh toggle_routine

rjmp move_routine
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

reset_routine:
nop
;;;;;;;;;;;;;;; TODO: CHANGE LOOKUP TABLE PTR TO ZERO
ldi DISP_REG, ZERO_DISP
rcall display
ldi DEC_REG, 0x00
rjmp main

toggle_routine:
nop
ldi DISP_REG, ONE_DISP
rcall display
rjmp main
;cpi DEC_REG, 0x00
;brne toggle_dec_off
;rjmp toggle_dec_on

move_routine:
nop
ld DISP_REG, Z+
rcall display
rjmp main

toggle_dec_on:
; Don't think this toggle actually adds the needed bit to the existing value right?
; need "set/clear bit in register" ?
ldi DEC_REG, 0x01
; TODO: why not just rjmp main. why another subroutine
rjmp toggle_end

toggle_dec_off:
ldi DEC_REG, 0x00
; TODO: why not just rnmp main. why another subroutine
rjmp toggle_end

toggle_end:
rjmp main

; Display subroutine that prints to the LCD the associate hex value in DISP_REG
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
add_dp:
ori DISP_REG, 0b00001000
rjmp dp_return

display:
cpi DEC_REG, 0x00
brne add_dp
dp_return:
; backup used registers on stack
push DISP_REG
push R17
in R17, SREG
push R17
ldi R17, 8 
; loop --> test all 8 bits
loop:
rol DISP_REG ;rotate left through Carry
BRCS set_ser_in_1 
; branch if Carry set
; put code here to set SER_IN to 0
cbi PORTB, SER_IN
rjmp end
set_ser_in_1:
; put code here to set SER_IN to 1
sbi PORTB, SER_IN
end:
; put code here to generate SRCK pulse
sbi PORTB, SRCK
nop
nop
cbi PORTB, SRCK
dec R17
brne loop
; put code here to generate RCK pulse
sbi PORTB, RCK
nop
nop
cbi PORTB, RCK
; restore registers from stack
pop R17
out SREG, R17
pop R17
pop DISP_REG
ret  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; subroutine for delay -- this isn't used in the final product
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
delay_long:
      ldi   r23,20      ; r23 <-- Counter for outer loop
  d1: ldi   r24,200     ; r24 <-- Counter for level 2 loop 
  d2: ldi   r25,200     ; r25 <-- Counter for inner loop
  d3: dec   r25
      nop              ; no operation 
      brne  d3 
      dec   r24
      brne  d2
      dec   r23
      brne  d1
      ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; subroutine to print out all digits
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
print_all:
ldi DISP_REG, ZERO_DISP
rcall display
rcall delay_long
ldi DISP_REG, ONE_DISP
rcall display
rcall delay_long
ldi DISP_REG, TWO_DISP
rcall display
rcall delay_long
ldi DISP_REG, THREE_DISP
rcall display
rcall delay_long
ldi DISP_REG, FOUR_DISP
rcall display
rcall delay_long
ldi DISP_REG, FIVE_DISP
rcall display
rcall delay_long
ldi DISP_REG, SIX_DISP
rcall display
rcall delay_long
ldi DISP_REG, SEVEN_DISP
rcall display
rcall delay_long
ldi DISP_REG, EIGHT_DISP
rcall display
rcall delay_long
ldi DISP_REG, NINE_DISP
rcall display
rcall delay_long
ldi DISP_REG, A_DISP
rcall display
rcall delay_long
ldi DISP_REG, B_DISP
rcall display
rcall delay_long
ldi DISP_REG, C_DISP
rcall display
rcall delay_long
ldi DISP_REG, D_DISP
rcall display
rcall delay_long
ldi DISP_REG, E_DISP
rcall display
rcall delay_long
ldi DISP_REG, F_DISP
rcall display
rcall delay_long
ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; end of program -- should never be reached due to main infinite loop
.exit
