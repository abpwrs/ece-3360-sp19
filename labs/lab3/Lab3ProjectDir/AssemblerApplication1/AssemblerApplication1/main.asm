////////////////////////////////////////////////////////////////////////////////
// Assembly language file for Lab 3 in ECE:3360 (Embedded Systems)
// Spring 2019, The University of Iowa.
//
// Desc: Lab3 Rotary Pulse Generator Controlled Duty-Cycle
//
// Authors: B. Mitchinson, A. Powers
////////////////////////////////////////////////////////////////////////////////



// Defenitions and Preprocessor Directives
//=============================================================================

// .inc include files
////////////////////////////////////////////////////////////////////////////////
.include "tn45def.inc"
////////////////////////////////////////////////////////////////////////////////


// set DDRB States -- we only have input pins for this lab (I think)
////////////////////////////////////////////////////////////////////////////////
cbi DDRB, 0
cbi DDRB, 1
sbi DDRB, 2
////////////////////////////////////////////////////////////////////////////////


// variables for current and previous state of the rpg
////////////////////////////////////////////////////////////////////////////////
// TODO: define a previous and current state register
.def current_state = r16
.def previous_state = r17
////////////////////////////////////////////////////////////////////////////////


// variables for duty-cycle range control
////////////////////////////////////////////////////////////////////////////////
// TODO: define a variable for the upper limit ~ based on the frequency on the lab webpage
.equ upper_cycle_limit = 254
// TODO: define a variable for the lower limit ~ same as above
.equ lower_cycle_limit = 2
.equ half_duty_cycle = 127
.def duty_reg = r18
////////////////////////////////////////////////////////////////////////////////


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



// variables for rotation speed
////////////////////////////////////////////////////////////////////////////////
// TODO: define a threshold between fast and slow rotation
// TODO: define a variable to store the state (fast or slow)
.def rate_reg = r19
ldi rate_reg, 0x0A
////////////////////////////////////////////////////////////////////////////////

//=============================================================================






// Subroutines and Program Logic
//=============================================================================

// main method (infinite update loop)
////////////////////////////////////////////////////////////////////////////////
ldi duty_reg, half_duty_cycle
main:
    nop
    // TODO: delay --> delay will likely need to vary if we have variable turning rate
	cbi PORTB, 2
	rcall delay_thirty
	nop
    rcall read_rpg
    nop
    rcall which_direction
    nop
	rcall disp_cycle
	sbi PORTB, 2 
	rcall delay_thirty
    rjmp main
////////////////////////////////////////////////////////////////////////////////



// all subroutines here, grouped by functionality
////////////////////////////////////////////////////////////////////////////////


// reading from rotary pulse generator
////////////////////////////////////////////////////////////////////////////////
read_rpg:
    nop
    push r28
    push r29
    ldi r28, 0x01
    ldi r29, 0x02

    mov previous_state, current_state
    ldi current_state, 0x00
    sbis PINB, 0
    add current_state, r29 ; run if a is high
    sbis PINB, 1
    add current_state, r28 ; run if b is high
    pop r29
    pop r28
    ret
////////////////////////////////////////////////////////////////////////////////


// deciding directions and main subroutines for each direction
////////////////////////////////////////////////////////////////////////////////
// figure out the direction the rpg is being turned
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
    nop

    // TODO: increase duty-cycle
    push r28
    mov r28, duty_reg
    add r28, rate_reg
    cpi r28, upper_cycle_limit
    brsh end_cwise
    add duty_reg, rate_reg
    end_cwise:
    pop r28
    ret

// counter_clockwise
counter_clockwise:
    nop
    // TODO: decrease duty-cycle
    push r28
    mov r28, duty_reg
    sub r28, rate_reg

    cpi r28, lower_cycle_limit
    brsh dec_ccwise
    rjmp end_ccwise

    dec_ccwise:
    sub duty_reg, rate_reg

    end_ccwise:
    pop r28

    ret

disp_cycle:
    nop
    ldi r28, 0xFF
    ldi r29, 0x00
    disp_loop:
    inc r29

    cp r29, duty_reg
    brsh disp_one
    cbi PORTB, 2
    rjmp disp_end

    disp_one:
    sbi PORTB, 2
	rjmp disp_end

    disp_end:

    cp r28, r29
    brne disp_loop

    ret


////////////////////////////////////////////////////////////////////////////////


// dealing with rate of turn?????
////////////////////////////////////////////////////////////////////////////////
// THIS IS FOR ALL THE MARBLES
////////////////////////////////////////////////////////////////////////////////

//=============================================================================

delay_thirty:
      ldi   r23,1      ; r23 <-- Counter for outer loop
  d1: ldi   r24,1    ; r24 <-- Counter for level 2 loop
  d2: ldi   r25,149     ; r25 <-- Counter for inner loop
  d3: dec   r25
      nop               ; no operation
      brne  d3
      dec   r24
      brne  d2
      dec   r23
      brne  d1
      ret

// exit main.asm
// (control should never reach this point as we have a main infinite loop)
.exit
