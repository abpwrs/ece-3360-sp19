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

// Input
int input[6] = {0,0,0,0,0,0};
float downSamples = 0;
float upSamples = 0;
char prevButtonDown = 0;
	
//  Button booleans
char buttonDown = 0;

// /////////////////////////////////

// Timer Config (For button sampling to register dits and dahs)
// /////////////////////////////////
void timer1_init(){
	// 8,000,000 / 256 = 31250 ticks per second.
	// We need timer to range from 0 to 31250
	//    @ a 1/256. It's overflow then occurs
	//    once every second.
	// 1  time  a second:              31250 : 0x7A12
	
	// Other scaled calculations
	// 40 times a second: 31250/40 =   782 : 0x30D
	
	TCCR1B = TCCR1B |  1 << CS12;   // 256 Pre-scale
	TCCR1B = TCCR1B |  1 << WGM12;  // CTC Mode: Clear timer on compare mode
	// When TCNT1 (Counter index) matches OCR1A, it's reset to zero
	// and the OCF1A Interrupt Flag is Set. OCF1A Automatically cleared
	OCR1A = 0x30D; // Set the top (Really bottom tho?)
	TIMSK1 = TIMSK1 |  1 << OCIE1A; // Output Compare A Match Interrupt Enable
}
// /////////////////////////////////

// Interrupt Configuration
// /////////////////////////////////

// button press interrupt
ISR(INT0_vect){
	PINC |= (1<<5); // Toggle Light
}

// Timer1 Overflow ISR
ISR(TIMER1_COMPA_vect){
	buttonDown = !(PORTC & (1<<5));
	
	if (buttonDown) {
		downSamples += 1;
		//lcd_printMsg("Down");
	}
	
	else {
		upSamples += 1;
		//lcd_printMsg("Up  ");
	}
	
	if ((buttonDown != prevButtonDown) && (buttonDown)) { // just pressed
		//lcd_printMsg("Press");
	}
	
	else if ((buttonDown != prevButtonDown) && (!buttonDown)) { // just released
		//lcd_printMsg("Release");
	}
	
	// store state for next operation
	prevButtonDown = buttonDown;
	
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
	lcd_init(LCD_DISP_ON_CURSOR);
	lcd_home();
	lcd_initText();
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
    _delay_ms(50);
	sei();
	
	// Turn on sampling timer
	timer1_init();

    // infinite loop
    while (1) 
    {
	    //lcd_printShort();
		//_delay_ms(1500);
		//lcd_printLong();
		//_delay_ms(1500);
		//lcd_printWord();
		//_delay_ms(1500);
    }
}
// ///////////////////////////////


// Quick Printing Methods
// ///////////////////////////////
void lcd_printShort(void) {
	lcd_gotoxy(9, 1);
	lcd_puts("Short");
}

void lcd_printLong(void) {
	lcd_gotoxy(9, 1);
	lcd_puts("Long ");
}

void lcd_printWord(void) {
	lcd_gotoxy(9, 1);
	lcd_puts("Word ");
}

void lcd_initText(void) {
	lcd_clrscr();
	lcd_puts("Msg:");
	lcd_gotoxy(0, 1);
	lcd_puts("Entered: ");
}

void lcd_printMsg(const char *s){
	lcd_gotoxy(5, 0);
	lcd_puts(s);
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
dit:             1 unit  (down)
dah:              3 units (down)
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
