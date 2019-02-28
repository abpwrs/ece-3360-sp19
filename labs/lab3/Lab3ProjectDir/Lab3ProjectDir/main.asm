////////////////////////////////////////////////////////////////////////////////
// Assembly language file for Lab 3 in ECE:3360 (Embedded Systems)
// Spring 2019, The University of Iowa.
//
// Desc: Lab3 Rotary Pulse Generator Controlled Duty-Cycle
//
// Authors: B. Mitchinson, A. Powers
////////////////////////////////////////////////////////////////////////////////

// definitions and preprocessor directives
//=============================================================================

// .inc include files
////////////////////////////////////////////////////////////////////////////////
.include "tn45def.inc"
////////////////////////////////////////////////////////////////////////////////
.cseg

// set DDRB
////////////////////////////////////////////////////////////////////////////////
cbi DDRB, 0
cbi DDRB, 1
sbi DDRB, 2
////////////////////////////////////////////////////////////////////////////////

// variables for current and previous state of the rpg
////////////////////////////////////////////////////////////////////////////////
.def current_state = r16
.def previous_state = r17
////////////////////////////////////////////////////////////////////////////////


// variables for duty-cycle range control
////////////////////////////////////////////////////////////////////////////////
.equ upper_cycle_limit = 162
.equ lower_cycle_limit = 30
.equ half_duty_cycle = 100
.def duty_reg = r18
ldi duty_reg, half_duty_cycle
////////////////////////////////////////////////////////////////////////////////

// variables for rotation speed
////////////////////////////////////////////////////////////////////////////////
.def rate_reg = r19
ldi rate_reg, 0x01
////////////////////////////////////////////////////////////////////////////////

// timer registers
////////////////////////////////////////////////////////////////////////////////
.def tmp1 = r23
.def tmp2 = r24
.def count = r25
ldi r30, 0x02
out TCCR0B, r30
ldi r30, 0x00

// rpg signal interpretation registers
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


// subroutines and program logic
//=============================================================================


// main method (infinite update loop)
////////////////////////////////////////////////////////////////////////////////

main:
    nop
    nop
    rcall read_rpg
    nop
    rcall which_direction
    nop

    cbi PORTB, 2

    ldi count, 206
    sub count, duty_reg
    rcall delay

    sbi PORTB, 2

    mov count, duty_reg
    rcall delay

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
    add duty_reg, rate_reg
    cpi duty_reg, upper_cycle_limit
    brsh recover_upper
    rjmp end_cwise
    recover_upper:
    ldi duty_reg, upper_cycle_limit
    end_cwise:
    ret

// counter_clockwise
counter_clockwise:
    sub duty_reg, rate_reg
    cpi duty_reg, lower_cycle_limit
    brsh end_ccwise
    ldi duty_reg, lower_cycle_limit
    end_ccwise:
    ret

////////////////////////////////////////////////////////////////////////////////

delay:
    ; Stop timer 0
    in tmp1, TCCR0B
    ldi tmp2, 0x00
    out TCCR0B, tmp2

    ; Clear over flow flag
    in tmp2, TIFR
    sbr tmp2, 1<<TOV0
    out TIFR, tmp2

    ; Start timer with new initial count
    out TCNT0, count
    out TCCR0B, tmp1

; wait
wait:
    in tmp2, TIFR
    sbrs tmp2, TOV0
    rjmp wait

    ret


// exit main.asm
// (control should never reach this point as we have a main infinite loop)
.exit
