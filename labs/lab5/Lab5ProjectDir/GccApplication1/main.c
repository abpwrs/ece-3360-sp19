// ///////////////////////////////////////////////////////////////////
// Assembly language file for Lab 5 in ECE:3360 (Embedded Systems)
// Spring 2019, The University of Iowa.
//
// Desc: Lab 5 Analog Data Logging System
//
// Authors: B. Mitchinson, A. Powers
// ///////////////////////////////////////////////////////////////////

// Setting CPU Clock Speed
#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz -> CProgramming Notes, Slide 10
#endif 
// ///////////////////////

// Required Imports
#include <avr/io.h>
#include <util/delay.h>
// ////////////////

// Function Prototypes
void setbaud();
void sendhi();
// transmits
void transmitenable();
void transmitdisable();
// prints 
void usart_prints(const char*);
void usart_printf();
// receive
void receiveenable();
void receivedisable();
// usart io
unsigned char read_char_from_pc();
void usart_init();
// ///////////////////


//unsigned char tmp; // puts this in SRAM
//static const char fdata[] PROGMEM = "Flash Gordon\n";  // String in Flash (Storing in flash is broken and idk why? Compile type?)
const char * sdata = "Enter 4 characters to reverse:"; // String in SRAM
const char * newline = "\n\r";
const char * hi = "Hi!\n\r";
char input;
char inputstring[5] = "";

int main(void)
{
	// DDRC = 0x20;			 // sbi DDRC, 5 : PORTC 5 Output
	
	usart_init();
	usart_prints(newline);

	usart_prints(sdata);
	int i = 0;
	while (i < 4){
		input = read_char_from_pc();
		inputstring[i] = input;
		i++;
	}

	usart_prints(newline);

	strrev(inputstring);
	usart_prints(inputstring);
	usart_prints(newline);

    while (1) 
    {
    }
}

// Set Asynchronous Normal Mode and configure BAUD Rate @ 9600
void usart_init(){
	UCSR0A = UCSR0A & ~(0x02);        // Set the mode to set "Async Normal Mode" (Slide 45 SerialComm)
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // set to transmit and receive
	UBRR0 = 0x33;				      // UBRR0 = [8000000 / 16(9600)] - 1 = 51.083 (51?)
}

// Enable transmit TXEN0 bit in UCSR0b
void transmitenable(){
	receivedisable();
	UCSR0B = UCSR0B | 0x08;		// Set TXEN0 (Bit 3) of UCSR0B
}

// Disable transmit TXEN0 bit in UCSR0b
void transmitdisable(){
	UCSR0B = UCSR0B & ~(0x08);  // Clear TXEN0 (Bit 3) of UCSR0B
}

// Enable receive RXEN0 bit in UCSR0B
void receiveenable(){
	transmitdisable();
	UCSR0B = UCSR0B | (1<<RXEN0);
}

// Disable receive RXEN0 bit in UCSR0B
void receivedisable(){
	UCSR0B = UCSR0B & (0<<RXEN0);
}

// Load one character into UDR0 for testing transfer
void sendhi(){
	UDR0 = 0x48;	// H
	_delay_ms(20);
	UDR0 = 0x69;	// i
	_delay_ms(20);
	UDR0 = 0x21;	// !
	_delay_ms(20);  // Delays do weird stuff based on chosen compile method,
				    //    so be careful. (Slide 17 + 27, CProgramming)
	UDR0 = 0x0A;	// Newline
	_delay_ms(20);
	UDR0 = 0x0D;	// Carriage Return
	_delay_ms(20);
}

void usart_prints(const char *sdata) {
	while (*sdata) {
		// Wait for UDRE0 to become set (==1), which indicates 
		// the UDR0 is empty and can receive the next character (Slide 46, Serial Comm)
		while (!(UCSR0A & (1<<UDRE0)));  // Option A
		//while (!(UCSR0A & (1<<TXC0))); // Option B
		UDR0 = *(sdata++);
	}
}

unsigned char read_char_from_pc(){
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}
