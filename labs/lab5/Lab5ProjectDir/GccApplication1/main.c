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
#include <stdio.h>
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
// using the adc
void adc_init();

unsigned short read_adc();
unsigned char read_command();
void interpret_command(const char*, const char *);
// ///////////////////


//unsigned char tmp; // puts this in SRAM
//static const char fdata[] PROGMEM = "Flash Gordon\n";  // String in Flash (Storing in flash is broken and idk why? Compile type?)
const char * prompt_for_command = "Enter a command $> "; // String in SRAM
const char * sdata = "Enter 4 characters to reverse:"; // String in SRAM
const char * newline = "\n\r";
const char * hi = "Hi!\n\r";
char input;
char inputstring[9] = "";

int main(void)
{
	// DDRC = 0x20;			 // sbi DDRC, 5 : PORTC 5 Output
	
	usart_init();
	adc_init();
	unsigned char command;
    while (1) 
    {
		command = 0x00;
		// command digits key
		// invalid command
		// invalid command --> 0x00
		// M --> 0x01
		// S:a,n,t --> 0x02
		// R:a,n --> 0x03
		// E:a,n,t,d --> 0x04
		//command = read_command(inputstring);
		//interpret_command(command, inputstring);
		usart_prints(newline);
		usart_prints(sdata);
		int i = 0;
		while (i < 4){
			input = read_char_from_pc();
			inputstring[i] = input;
			i++;
		}
		strrev(inputstring);
		usart_prints(newline);
		usart_prints(inputstring);

		usart_prints(newline);
		unsigned short adc_output = read_adc();
		unsigned short enough = (unsigned short) ((ceil(log10(adc_output))+1)*sizeof(char));
		char adc_buff[enough];
		sprintf(adc_buff, "%d", adc_output);
		usart_prints(adc_buff);
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

// Configure ADC
void adc_init(){
	ADMUX = ADMUX | (1<<REFS0); // Configure ADC Reference
	ADCSRA = (1<<ADEN);
}

unsigned short read_adc(){
	ADCSRA = ADCSRA | (1<<ADSC);
	while(ADCSRA & (1<<ADIF));
	return ADCW;
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

// command digits key
// invalid command
// invalid command --> 0x00
// M --> 0x01
// S:a,n,t --> 0x02
// R:a,n --> 0x03
// E:a,n,t,d --> 0x04
void interpret_command(const char * command_code, const char * command_string){
	usart_prints(newline);
	usart_prints(newline);
	usart_prints("Command was: ");
	usart_prints(newline);
	usart_prints(command_string);
	usart_prints(newline);
	usart_prints(newline);
	
}

// command digits key
// invalid command
// invalid command --> 0x00
// M --> 0x01
// S:a,n,t --> 0x02
// R:a,n --> 0x03
// E:a,n,t,d --> 0x04
unsigned char read_command(char * command_array){
	usart_prints(newline);
	usart_prints(prompt_for_command);
	command_array = "";

	unsigned char first_char = "";
	first_char = read_char_from_pc();

	unsigned short max_command_length = 0x00;
	unsigned short ret_code = 0x00;

	if (first_char == 'M') {
		command_array[0] = first_char;
		max_command_length = 0x00;
		ret_code = 0x01;
	} else if (first_char == 'S') {
		command_array[0] = first_char;
		max_command_length = 0x07;
		ret_code = 0x02;

	} else if (first_char == 'R') {
		command_array[0] = first_char;
		max_command_length = 0x05;
		ret_code = 0x03;

	} else if (first_char == 'E') {
		command_array[0] = first_char;
		max_command_length = 0x09;
		ret_code = 0x04;
	}	 

	for (int i = 1; i < max_command_length; ++i) {
		command_array[i] = read_char_from_pc();
	}

	return ret_code;
}



// OLD CODE --> reverse string
// const char * sdata = "Enter 4 characters to reverse:"; // String in SRAM
//		int i = 0;
//		while (i < 4){
//			input = read_char_from_pc();
//			inputstring[i] = input;
//			i++;
//		}
//		strrev(inputstring);
//		usart_prints(newline);
//		usart_prints(inputstring);