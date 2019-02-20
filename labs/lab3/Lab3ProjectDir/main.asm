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
.equ upper_cycle_limit = 70
// TODO: define a variable for the lower limit ~ same as above
.equ lower_cycle_limit = 30
// TODO: define a register to hold the current value of the duty cycle?
.def duty_reg = r18
////////////////////////////////////////////////////////////////////////////////


// variables for rotation speed
////////////////////////////////////////////////////////////////////////////////
// TODO: define a threshold between fast and slow rotation
// TODO: define a variable to store the state (fast or slow)
////////////////////////////////////////////////////////////////////////////////

//=============================================================================






// Subroutines and Program Logic
//=============================================================================

// main method (infinite update loop) 
////////////////////////////////////////////////////////////////////////////////
main:
    nop
    // TODO: delay --> delay will likely need to vary if we have variable turning rate
	rcall read_rpg
	nop
	rcall which_direction
	// rcall disp_cycle
	// sbi PINB, 2
    rjmp main
////////////////////////////////////////////////////////////////////////////////



// all subroutines here, grouped by functionality
////////////////////////////////////////////////////////////////////////////////


// 0 - both on   0b00000000
// 1 - clockwise  0b00000001
// 2 - counter    0b00000010
// 3 - both off    0b00000011
// A is bit position 1
// B is ibit position 0


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
	// TODO: figure out direction
	// TODO: call corresponding subroutine based on direction
	cpi previous_state, 0x00
	breq which_end

	// if current state low
	cpi current_state, 0x03
	breq current_low
	rjmp which_end

	current_low:
		cpi previous_state, 0x01
		breq clockwise
		cpi previous_state, 0x02
		breq counter_clockwise
		rjmp which_end

	which_end:

	ret


// clockwise
clockwise:
    nop
	// TODO: increase duty-cycle
	sbi PORTB, 2
	ret

// counter_clockwise
counter_clockwise:
    nop
	// TODO: decrease duty-cycle
	cbi PORTB, 2
	ret

disp_cycle:
	nop
	ret

////////////////////////////////////////////////////////////////////////////////


// dealing with rate of turn?????
////////////////////////////////////////////////////////////////////////////////
// THIS IS FOR ALL THE MARBLES 
////////////////////////////////////////////////////////////////////////////////

//=============================================================================



// exit main.asm 
// (control should never reach this point as we have a main infinite loop)
.exit



