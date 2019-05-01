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
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"
#include "USART_RS232_H_file.h"
// /////////////////////////////////

// LCD #define statements
// /////////////////////////////////
// Command mode vs Data mode for LCD
#ifndef RS  
#define RS  5
#endif
// Enable pin of LCD
#ifndef E   
#define E   3
#endif
// Data pin 4 of LCD
#ifndef D4  
#define D4  0
#endif
// Data pin 5 of LCD
#ifndef D5  
#define D5  1
#endif
// Data pin 6 of LCD
#ifndef D6  
#define D6  2
#endif
// Data pin 7 of LCD
#ifndef D7  
#define D7  3
#endif
// LED is on PORTC pin 5
#ifndef LED_PORT  
#define LED_PORT  5
#endif
// maximal morse code array length
#ifndef MORSE_ARR_LEN 
#define MORSE_ARR_LEN 6
#endif
// Baud rate of USART communication
#ifndef BAUDRATE 
#define BAUDRATE 9600
#endif
// Port that the bluetooth pins are located on
#ifndef BLUE_TOOTH_PORT 
#define BLUE_TOOTH_PORT PORTD
#endif
// Voltage pin of the HC-05 bluetooth module
#ifndef BLUE_TOOTH_VCC 
#define BLUE_TOOTH_VCC 7
#endif
// Enable pin of the HC-05 bluetooth module
#ifndef BLUE_TOOTH_E 
#define BLUE_TOOTH_E 6
#endif
// /////////////////////////////////

// Globals
// /////////////////////////////////

// Farnsworth unit speed
#ifndef F_UNIT 
#define F_UNIT 3.0 // 4 units per second (250ms)
#endif
// samplings per unit
#ifndef SAMPLES_PER_UNIT 
#define SAMPLES_PER_UNIT 26 // @ 80/second samples 80/fUnit = 26
#endif

// Character mapping (dictionary out of two array cross order indexed)
#ifndef TOTAL_CHARS 
#define TOTAL_CHARS 40
#endif
int keys[TOTAL_CHARS] = {243, 486, 405, 360, 324, 648, 567, 594, 675, 621, 702, 432, 351, 378, 459, 603, 630, 387, 477, 441, 468, 693, 369, 612, 639, 684, 726, 483, 402, 375, 366, 363, 606, 687, 714, 723, 692, 455, 400, 364};
char chars[TOTAL_CHARS] = {'E', 'T', 'A', 'H', 'I', 'M', 'N', 'D', 'G', 'K', 'O', 'R', 'S', 'U', 'W', 'B', 'C', 'F', 'J', 'L', 'P', 'Q', 'V', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ',', '.', '?', ' '};

// Input
volatile int input[MORSE_ARR_LEN] = {0,0,0,0,0,0};
volatile int inputIndex = 0;
char too_long = 0x00;
float downSamples = 0;
float upSamples = 0;

// State for edge detection
char prevButtonDown = 0;

// Boolean to improve entry
char justFinishedChar = 1;
	
//  Button booleans
char buttonDown = 0;

// /////////////////////////////////

// Prototypes
// /////////////////////////////////
char hash_inputs(void);
void lcd_show_input_arr(void);
void lcd_clr_top(void);
void lcd_clr_bot(void);
void lcd_dah(void);
void lcd_dit(void);
void lcd_bad(void);
void reset_input_arr(void);

// bluetooth functions
void power_on_hc05(void);
void power_off_hc05(void);
void enable_on_hc05(void);
void enable_off_hc05(void);
void blue_tooth_to_command_mode(void);

////////////////////////////////////

// Timer Config (For button sampling to register dits and dahs)
// /////////////////////////////////
void timer1_init(void){
	// 8,000,000 / 256 = 31250 ticks per second.
	// We need timer to range from 0 to 31250
	//    @ a 1/256. It's overflow then occurs
	//    once every second.
	// 1  time  a second:              31250 : 0x7A12
	
	// Other scaled calculations
	// 40 times a second: 31250/40 =   782 : 0x30D
	// 80 times a second: 31250/80 =   391 : 0x187
	
	TCCR1B = TCCR1B |  1 << CS12;   // 256 Pre-scale
	TCCR1B = TCCR1B |  1 << WGM12;  // CTC Mode: Clear timer on compare mode
	// When TCNT1 (Counter index) matches OCR1A, it's reset to zero
	// and the OCF1A Interrupt Flag is Set. OCF1A Automatically cleared
	OCR1A = 0x187; // Set the top
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
	
	buttonDown = !(PIND & (1<<2));
	
	if (buttonDown) {
		downSamples += 1;
		if (downSamples >= 6 * SAMPLES_PER_UNIT){
			justFinishedChar = 1;
			reset_input_arr();
			lcd_show_input_arr();
			too_long = 0x01;
		}
	}

	else {
		upSamples += 1;
		if ((upSamples >= 5 * SAMPLES_PER_UNIT) && (!justFinishedChar)) {
			lcd_show_input_arr();
			char inputChar = hash_inputs();
			lcd_gotoxy(6,1);
			lcd_putc(':');
			lcd_putc(inputChar);
			if (inputChar != '!'){
				USART_TxChar(inputChar);
			}
			justFinishedChar = 1;
			reset_input_arr();
		}
	}
	if ((buttonDown != prevButtonDown) && (buttonDown)) { // just pressed
		justFinishedChar = 0;
		downSamples = 0;
		upSamples = 0;
	}
	else if ((buttonDown != prevButtonDown) && (!buttonDown) && (inputIndex < 6)) { // just released + have room for more entries
		if (too_long){
			too_long = 0x00;
		}
		else if (downSamples >= 1 * SAMPLES_PER_UNIT){
			lcd_dah();
			input[inputIndex] = 2;
			inputIndex++;
			lcd_show_input_arr();
		}
		else { //(downSamples >= 1 * samplesPerUnit){
			lcd_dit();
			input[inputIndex] = 1;
			inputIndex++;
			lcd_show_input_arr();
		}
		downSamples = 0;
		upSamples = 0;
	}
	
	// store state for next operation, in order to identify pos and neg edges
	prevButtonDown = buttonDown;
	if (too_long == 0x01){
		reset_input_arr();
		
		lcd_show_input_arr();
	}
}

// /////////////////////////////////

// Main Loop
// ///////////////////////////////
int main(void)
{
    //LCD Data Direction Configuration
    DDRC |= (1<<D4) | (1<<D5) | (1<<D6) | (1<<D7) ;
	DDRB |= (1<<RS) | (1<<E);
	DDRD |= (1<<PD6) | (1<<PD7);

	// Initial LCD Config
	lcd_init(LCD_DISP_ON_CURSOR);
	lcd_gotoxy(0,0);
	lcd_puts("Msg:");
	
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
	USART_Init(BAUDRATE); // initialize USART with 9600 baud rate 
	
	blue_tooth_to_command_mode();
	
	//SUSART_SendString("AT\r\n");
	//USART_SendString("AT+NAME=M\r\n");
	//USART_SendString("AT+NAME?\r\n");
	//USART_SendString("AT+ADDR?\r\n");
	//USART_SendString("AT+ROLE?\r\n");
	//USART_SendString("AT+ROLE=1\r\n");
	//USART_SendString("AT+BIND=14,3,5fa85\r\n"); // Addr of slave: used to program master (ted's)
	//USART_SendString("AT+BIND=14,3,5f6f4\r\n"); // Addr of master: used to program slave (ours)
	//USART_SendString("AT+BIND?\r\n");
	//USART_SendString("AT+CMODE=0\r\n");
	
	char data_in;
	int num_chars = 4;

    // infinite loop
    while (1) 
    {
		// this command is blocking, so all interrupts happen here
		data_in = USART_RxChar();
		// disable interrupts while writing to LCD
		cli();
		// check if we need to wrap around to the beginning of the line
		if (num_chars > 15){
			lcd_clr_top();
			num_chars = 4;
		}
		lcd_gotoxy(num_chars, 0);
		if (data_in != '\r' && data_in != '\n'){
			lcd_putc(data_in);
			num_chars++;
		} else {
			lcd_putc(' ');
			num_chars++;
		}
		sei();
    }
} 

// ///////////////////////////////
// https://www.geeksforgeeks.org/convert-base-decimal-vice-versa/
int toDecimal(volatile int *arr, int base) 
{ 
    int power = 1;
    int num = 0;  

    for (int i = MORSE_ARR_LEN - 1; i >= 0; i--) 
    { 
        if (arr[i] >= base) 
        { 
           printf("Invalid Number"); 
           return -1; 
        } 
        num += arr[i] * power; 
        power = power * base; 
    } 
    return num; 
} 

// ///////////////////////////////

// bluetooth functions
// ///////////////////////////////
void power_on_hc05(void) {
	BLUE_TOOTH_PORT |= (1<<BLUE_TOOTH_VCC);
}
void power_off_hc05(void) {
	BLUE_TOOTH_PORT &= ~(1<<BLUE_TOOTH_VCC);
}
void enable_on_hc05(void) {
	BLUE_TOOTH_PORT |= (1<<BLUE_TOOTH_E);
}
void enable_off_hc05(void) {
	BLUE_TOOTH_PORT &= ~(1<<BLUE_TOOTH_E);
}
void blue_tooth_to_command_mode(void) {
	enable_off_hc05();
	power_on_hc05();
	_delay_ms(2000);
	enable_on_hc05();
	_delay_ms(3000);
	enable_off_hc05();
}

// ///////////////////////////////

// Quick Printing Methods
// ///////////////////////////////
void lcd_clr_top(void){
	lcd_gotoxy(0,0);
	lcd_puts("Msg:            ");
}

void lcd_clr_bot(void){
	lcd_gotoxy(0,1);
	lcd_puts("                ");
}

void lcd_dah(void){
	lcd_gotoxy(13,1);
	lcd_puts("Dah");
}

void lcd_dit(void){
	lcd_gotoxy(13,1);
	lcd_puts("Dit");
}

void lcd_bad(void){
	lcd_gotoxy(13,1);
	lcd_puts("Bad");
}

void lcd_clear_bot_three(void){
	lcd_gotoxy(13,1);
	lcd_puts("   ");
}
// ///////////////////////////////

// Reveal input array
void lcd_show_input_arr(void){
	// Showing the input array adds 20% to memory due to sprintf
	char buff[7];
	int i=0;
	int index = 0;
	for (i=0; i<6; i++)
		index += sprintf(&buff[index], "%d", input[i]);
	lcd_gotoxy(0, 1);
	lcd_puts("        ");
	lcd_gotoxy(0, 1);
	lcd_puts(buff);
}

// Get character from input array
char hash_inputs(void){
	int lookupKey = toDecimal(input, 3);
	int i = 0;
	while(keys[i] != lookupKey){
		i++;
		if (i >= TOTAL_CHARS){
			lcd_bad();
			return '!';
		}
	}
	return chars[i];
}

void reset_input_arr(void){
	for(int i = 0; i<6; i++){
		input[i] = 0;
	}
	inputIndex = 0;
}

/*
Farnsworth Timing (Altered a lot based on user preference. Pretty clear in the ISR)
dit:             1 unit  (down)
dah:              3 units (down)
morse-character:   1 unit  (up)
ascii-character:   3 units (up)
Word:              7 units or more (up)
*/
