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
#define RS  5
#define E   3
#define D4  0
#define D5  1
#define D6  2
#define D7  3
#define LED_PORT  5
#define MORSE_ARR_LEN 6
#define BAUDRATE 9600
#define BLUE_TOOTH_PORT PORTD
#define BLUE_TOOTH_VCC 7
#define BLUE_TOOTH_E 6
// /////////////////////////////////

// Globals
// /////////////////////////////////

// Farnsworth unit speed
#define F_UNIT 3.0 // 4 units per second (250ms)
#define SAMPLES_PER_UNIT 26 // @ 80/second samples 80/fUnit = 26

// Character mapping (dictionary out of two array cross order indexed)
int primes[] = {5, 7, 13, 17, 19, 23};
char chars[39] = {'E','T','A','H','I','M','N','D','G','K','O','R','S','U','W','B','C','F','J','L','P','Q','V','X','Y','Z','0','1','2','3','4','5','6','7','8','9',',','.','?'};
int keys[39] = {5,10,19,42,12,24,17,30,37,43,50,32,25,38,45,47,60,55,79,49,62,71,59,64,77,54,122,117,110,97,80,61,66,73,86,103,138,131,114};

// Input
volatile int input[MORSE_ARR_LEN] = {0,0,0,0,0,0};
volatile int inputIndex = 0;

float downSamples = 0;
float upSamples = 0;

// State for edge detection
char prevButtonDown = 0;

// Boolean to improve entry
char justFinishedWord = 1;
char justFinishedChar = 1;
	
//  Button booleans
char buttonDown = 0;

// /////////////////////////////////

// Prototypes
// /////////////////////////////////
char hashInputs();
void lcd_initText();
void lcd_printEntered();
void lcd_printMsg();
void lcd_showInputArr();
void lcd_strobe();
void resetInputArr();

// bluetooth functions
void power_on_hc05(void);
void power_off_hc05(void);
void enable_on_hc05(void);
void enable_off_hc05(void);
void blue_tooth_to_command_mode(void);

////////////////////////////////////

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
	buttonDown = !(PINC & (1<<5));
	
	if (buttonDown) {
		downSamples += 1;
		if (downSamples >= 4 * SAMPLES_PER_UNIT){
			lcd_printMsg("Too Long");
			PORTC |= (1<<5);
			justFinishedChar = 1;
			justFinishedWord = 1;
			resetInputArr();
		}
		//lcd_printMsg("Down");
	}
	else {
		upSamples += 1;
		if ((upSamples >= 20 * SAMPLES_PER_UNIT) && (!justFinishedWord)) {
			lcd_printMsg("Word Done");
			justFinishedWord = 1;
			lcd_gotoxy(8, 1);
			lcd_puts("        ");
			lcd_puts("  ");
		}
		else if ((upSamples >= 5 * SAMPLES_PER_UNIT) && (!justFinishedChar)) {
			lcd_printMsg("Char Done");
			lcd_showInputArr();
			char inputChar = hashInputs();
			lcd_gotoxy(14,1);
			lcd_putc(':');
			lcd_putc(inputChar);
			USART_TxChar(inputChar);
			justFinishedChar = 1;
			resetInputArr();
		}
		//lcd_printMsg("Up  ");
	}
	if ((buttonDown != prevButtonDown) && (buttonDown)) { // just pressed
		justFinishedWord = 0;
		justFinishedChar = 0;
		downSamples = 0;
		upSamples = 0;
		//lcd_printMsg("Press");
	}
	else if ((buttonDown != prevButtonDown) && (!buttonDown) && (inputIndex < 6)) { // just released + have room for more entries
		if (downSamples >= 1 * SAMPLES_PER_UNIT){
			lcd_printMsg("Dah");
			input[inputIndex] = 2;
			inputIndex++;
			lcd_showInputArr();
		}
		else { //(downSamples >= 1 * samplesPerUnit){
			lcd_printMsg("Dit");
			input[inputIndex] = 1;
			inputIndex++;
			lcd_showInputArr();
		}
		//else {
			//lcd_printMsg("Too Short");
			//justFinishedChar = 1;
			//justFinishedWord = 1;
			//resetInputArr();
		//}
		downSamples = 0;
		upSamples = 0;
		//lcd_printMsg("Release");
	}
	
	// store state for next operation, in order to identify pos and neg edges
	prevButtonDown = buttonDown;
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
	lcd_home();
	// lcd_initText();
	
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
	//USART_SendString("AT+ROLE=1\r\n");
	//SUSART_SendString("AT\r\n");
	//USART_SendString("AT+NAME=M\r\n");
	//USART_SendString("AT+NAME?\r\n");
	//USART_SendString("AT+ADDR?\r\n");
	USART_SendString("HELLO");
	char data_in;
	int num_chars = 0;

    // infinite loop
    while (1) 
    {
		data_in = USART_RxChar();
		if (data_in != '\r' && data_in != '\n'){
			lcd_putc(data_in);
			num_chars++;
		} else {
			lcd_putc(' ');
			num_chars++;
		}

		if (num_chars > 15){
			lcd_gotoxy(0,1);
			num_chars = 0;
		}

	   
    }
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

void lcd_initText(void) {
	lcd_clrscr();
	lcd_home();
	lcd_puts("Msg:");
	lcd_gotoxy(0, 1);
	lcd_puts("Entered:");
}

void lcd_printMsg(const char *s){
	lcd_gotoxy(5, 0);
	lcd_puts("           ");
	lcd_gotoxy(5, 0);
	lcd_puts(s);
}

void lcd_printEntered(const char *s){
	lcd_gotoxy(8, 1);
	lcd_puts("        ");
	lcd_gotoxy(8, 1);
	lcd_puts(s);
}
// ///////////////////////////////

// Reveal input array
void lcd_showInputArr(){
	// Showing the input array adds 20% to memory due to sprintf
	char buff[7];
	int i=0;
	int index = 0;
	for (i=0; i<6; i++)
		index += sprintf(&buff[index], "%d", input[i]);
	lcd_printEntered(buff);
}

// Get character from input array
char hashInputs(){
	int i;
	int lookupKey = 0;
	for (i = 0; i < 6; i++){
		lookupKey += primes[i]*input[i];
	}
	i = 0;
	while(keys[i] != lookupKey){
		i++;
		if (i >= 39){
			lcd_printEntered("BadMsg");
			return '!';
		}
	}
	return chars[i];
}

void resetInputArr(void){
	for(int i = 0; i<6; i++){
		input[i] = 0;
	}
	inputIndex = 0;
}

// strobe the lcd
void lcd_strobe(void){
    PORTB &= ~(1<<E);
    _delay_us(200);
    PORTB |= (1<<E);
    _delay_us(200);
    PORTB &= ~(1<<E);
}

/*
Farnsworth Timing (Altered a lot based on user preference. Pretty clear in the ISR)
dit:             1 unit  (down)
dah:              3 units (down)
morse-character:   1 unit  (up)
ascii-character:   3 units (up)
Word:              7 units or more (up)
*/












// bluetooth blinky
// http://www.electronicwings.com/avr-atmega/hc-05-bluetooth-module-interfacing-with-atmega1632
/*
#include <avr/io.h>
#include "USART_RS232_H_file.h"	

#define LED PORTC		

int main(void)
{
	char Data_in;
	DDRC = 0xff;		
	USART_Init(9600);	
	LED = 0;
	
	while(1)
	{
		Data_in = USART_RxChar();	
		if(Data_in =='1')
		{
			LED |= (1<<PC5);	
			USART_SendString("LED_ON");
			
		}
		else if(Data_in =='2')
		{
			LED &= ~(1<<PC5);	
			USART_SendString("LED_OFF");
		}
		else
		USART_SendString("Select proper option"); 
	}
}
*/

// bluetooth hello world main
/*
#define LED PORTC
int main(void)
{
	char Data_in;
	DDRC = 0xff;		// make PORT as output port 
	lcd_init(LCD_DISP_ON_CURSOR);
	lcd_home();
	USART_Init(9600);	// initialize USART with 9600 baud rate 
	LED = 0;
	int chars = 0;
	while(1)
	{
		
		
		Data_in = USART_RxChar();	         // receive data from Bluetooth device
		lcd_putc(Data_in);
		chars++;
		if (chars > 15){
			lcd_clrscr();
			chars = 0;
			lcd_home();
			USART_SendString("FULL!");
		}
	}
} 

*/