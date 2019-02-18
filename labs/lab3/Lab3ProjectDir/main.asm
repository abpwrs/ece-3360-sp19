////////////////////////////////////////////////////////////////////////////////
// Assembly language file for Lab 3 in ECE:3360 (Embedded Systems)
// Spring 2019, The University of Iowa.
//
// Desc: 
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
// TODO: set states here
////////////////////////////////////////////////////////////////////////////////


// variables for current and previous state of the rpg
////////////////////////////////////////////////////////////////////////////////
// TODO: define a previous and current state register 
////////////////////////////////////////////////////////////////////////////////


// variables for duty-cycle range control
////////////////////////////////////////////////////////////////////////////////
// TODO: define a variable for the upper limit ~ based on the frequency on the lab webpage
// TODO: define a variable for the lower limit ~ same as abovevv
// TODO: define a register to hold the current value of the duty cycle?
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


    rjmp main
////////////////////////////////////////////////////////////////////////////////



// all subroutines here, grouped by functionality
////////////////////////////////////////////////////////////////////////////////


// reading from rotary pulse generator
////////////////////////////////////////////////////////////////////////////////
read_rpg:
    nop
	// TODO: set specified registers given what is read in on pins YYY and ZZZ
    ret
////////////////////////////////////////////////////////////////////////////////


// deciding directions and main subroutines for each direction
////////////////////////////////////////////////////////////////////////////////
// figure out the direction the rpg is being turned
which_direction:
    nop
	// TODO: figure out direction
	// TODO: call corresponding subroutine based on direction
	ret


// clockwise
clockwise:
    nop
	// TODO: increase duty-cycle
	ret

// counter_clockwise
counter_clockwise:
    nop
	// TODO: decrease duty-cycle
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



