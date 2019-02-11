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
.cseg
.org 0

; 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.equ SER_IN = 0
.equ SRCK = 1
.equ RCK = 2
.equ PUSH_BUTTON = 4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; main method -- infinite loop to keep the controller responding to input
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
main:
nop
sbis PINB, PUSH_BUTTON ; if button pushed, next line will execute
rcall button_pressed ; react to the button press
;rcall reset_disp
rjmp main
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
button_pressed:
ldi R18, 0x00 ; set initial state of counter to zero
rcall count_press
rjmp update

; count press sets certain register values associated with the length of button press
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
count_press:
rcall my_delay
cpi R18, 255
breq skip
inc R18
skip:
sbis PINB, PUSH_BUTTON ; if button is still pressed, execute next line
rjmp count_press ; rjmp to this method
ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; my delay of 10000 cycles -- currently ~ 10032 cycles
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
my_delay:
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

; update the state based on the contents of R17
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
update:
ldi R19, 0x64
ldi R20, 0xc8

cp R18, R20
brsh reset_routine

cp R18, R19
brsh toggle_routine

rjmp move_routine
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

reset_routine:
nop
ldi R16, ZERO_DISP
rcall display
; rcall delay_long
ldi R18, 0x00 ; set initial state of counter to zero
rjmp main

toggle_routine:
nop
ldi R16,  ONE_DISP
rcall display
; rcall delay_long
ldi R18, 0x00 ; set initial state of counter to zero
rjmp main

move_routine:
nop
ldi R16, TWO_DISP
rcall display
;rcall delay_long
ldi R18, 0x00 ; set initial state of counter to zero
rjmp main



; Display subroutine that prints to the LCD the associate hex value in R16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
display:
; backup used registers on stack
push R16
push R17
in R17, SREG
push R17
ldi R17, 8 
; loop --> test all 8 bits
loop:
rol R16 ;rotate left trough Carry
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
pop R16
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
ldi R16, ZERO_DISP
rcall display
rcall delay_long
ldi R16, ONE_DISP
rcall display
rcall delay_long
ldi R16, TWO_DISP
rcall display
rcall delay_long
ldi R16, THREE_DISP
rcall display
rcall delay_long
ldi R16, FOUR_DISP
rcall display
rcall delay_long
ldi R16, FIVE_DISP
rcall display
rcall delay_long
ldi R16, SIX_DISP
rcall display
rcall delay_long
ldi R16, SEVEN_DISP
rcall display
rcall delay_long
ldi R16, EIGHT_DISP
rcall display
rcall delay_long
ldi R16, NINE_DISP
rcall display
rcall delay_long
ldi R16, A_DISP
rcall display
rcall delay_long
ldi R16, B_DISP
rcall display
rcall delay_long
ldi R16, C_DISP
rcall display
rcall delay_long
ldi R16, D_DISP
rcall display
rcall delay_long
ldi R16, E_DISP
rcall display
rcall delay_long
ldi R16, F_DISP
rcall display
rcall delay_long
ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


reset_disp:
ldi R16, 0x00
rcall display
ret

; end of program -- should never be reached due to main infinite loop
.exit
