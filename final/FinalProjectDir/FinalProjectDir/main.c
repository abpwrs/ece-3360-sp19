// ///////////////////////////////////////////////////////////////////
// Main C Final Project file for ECE:3360 (Embedded Systems)
// Spring 2019, The University of Iowa.
//
// Desc: Morse Code Transmission Device (Client + Receiver Pair)
//
// Authors: B. Mitchinson, A. Powers
// ///////////////////////////////////////////////////////////////////

// Setting CPU Clock Speed
#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz -> CProgramming Notes, Slide 10
#endif
// ///////////////////////

// Imports
// /////////////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"
// /////////////////////////////////

// LCD #define statements
// /////////////////////////////////
#define RS  5
#define E   3
#define D4  0
#define D5  1
#define D6  2
#define D7  3
#define LED_PORT  5
#define MORSE_ARR_LEN 6
// /////////////////////////////////

// Globals
// /////////////////////////////////
// Button States
// UP = 0x01
// DOWN = 0x00
char prev_state = 0x00;
char curr_state = 0x00;
int  press_len = 0;
// /////////////////////////////////

// timer that does a thing TBD
// interrupt HERE
// reset the timer

// Timer Config
// /////////////////////////////////

// /////////////////////////////////

// Interrupt Configuration
// /////////////////////////////////
// button press interrupt
ISR(INT0_vect){
    if (PORTC & (1<<5)){  // if off
        PORTC &= ~(1<<5); // Turn on
    } else {
       PORTC |= (1<<5);   // Turn off
    }
}
// /////////////////////////////////

// Main Loop
// ///////////////////////////////
int main(void)
{
    // LCD Data Direction Configuration
    DDRC |= (1<<D4);
    DDRC |= (1<<D5);
    DDRC |= (1<<D6);
    DDRC |= (1<<D7);
    DDRB |= (1<<RS);
    DDRB |= (1<<E);
	
	// Initial LCD Config
	lcd_init(LCD_DISP_ON_CURSOR_BLINK);
	lcd_home();
	//lcd_puts("abc");
	//lcd_putc('B');
	//lcd_putc('Z');
	//lcd_gotoxy(4,1);
	//lcd_putc('Z');
	
	// Turn off the LED to use for button feedback
	PORTC |= (1<<5);
	
    // set interrupt configurations
    DDRC |= 1 << 5;
    EICRA |= (1<<ISC00);
    EIMSK |= (1<<INT0);

    // enable interrupts
    sei();

    // infinite loop
    while (1) 
    {
	    
    }
}
// ///////////////////////////////


// strobe the lcd
void lcd_strobe(void){
    PORTB &= ~(1<<E);
    _delay_us(200);
    PORTB |= (1<<E);
    _delay_us(200);
    PORTB &= ~(1<<E);
}

// convert 6 integer array to ascii
char morse_to_ascii(int * morse_arr, int used_len){
    switch (used_len){
        case 0:
            // error
            break;

        case 1: 
            // lookup table 1
            break;

        case 2: 
            // lookup table 2
            break;
        case 3: 
            // lookup table 3
            break;

        case 4: 
            // lookup table 4
            break;

        case 5: 
            // lookup table 5
            break;

        case 6: 
            // lookup table 6
            break;

        default:
            // error
            break;
    }
}

/*
Farnsworth Timing:
short:             1 unit  (down)
long:              3 units (down)
morse-character:   1 unit  (up)
ascii-character:   3 units (up)
Word:              7 units or more (up)
*/

/*
ASCII, length 
45, 1
54, 1
41, 2
49, 2
4D, 2
4E, 2
44, 3
47, 3
4B, 3
4F, 3
52, 3
53, 3
55, 3
57, 3
42, 4
43, 4
46, 4
48, 4
4A, 4
4C, 4
50, 4
51, 4
56, 4
58, 4
59, 4
5A, 4
30, 5
31, 5
32, 5
33, 5
34, 5
35, 5
36, 5
37, 5
38, 5
39, 5
2E, 6
2C, 6
3F, 6
*/
