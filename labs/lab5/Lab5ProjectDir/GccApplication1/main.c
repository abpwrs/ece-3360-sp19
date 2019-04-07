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
#include <avr/interrupt.h>
#include <avr/eeprom.h>
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
void print_new_line();
// receive
void receiveenable();
void receivedisable();
// usart io
unsigned char read_char_from_pc();
void usart_init();
// using the adc
void adc_init();


int read_adc();
unsigned int read_command(char * command_array, size_t arr_len);
void interpret_command(const int, const char *);
// ///////////////////

int main(void)
{
	// DDRC = 0x20;			 // sbi DDRC, 5 : PORTC 5 Output
	
	usart_init();
	adc_init();
	const size_t arr_len = 9;
	char inputstring[arr_len];
	unsigned int command;

    eeprom_write_word(0x00, 4387);
	int result = eeprom_read_word(0x00);
	char thingy[20];
	sprintf(thingy, "Got: %d", result);
	usart_prints(thingy);

	print_new_line();

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
		command = read_command(inputstring, arr_len);
		interpret_command(command, inputstring);
		char t_buff[10];
		sprintf(t_buff, "%d", sizeof(5000));
		usart_prints(t_buff);
	}
}

// Set Asynchronous Normal Mode and configure BAUD Rate @ 9600
void usart_init(){
	UCSR0A = UCSR0A & ~(0x02);        // Set the mode to set "Async Normal Mode" (Slide 45 SerialComm)
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // set to transmit and receive
	UBRR0 = 0x33;				      // UBRR0 = [8000000 / 16(9600)] - 1 = 51.083 (51?)
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

// Configure ADC
void adc_init(){
	ADMUX = ADMUX | (1<<REFS0); // Configure ADC Reference
	ADCSRA = (1<<ADEN);
}

int read_adc(){
	ADCSRA = ADCSRA | (1<<ADSC);
	while(ADCSRA & (1<<ADIF));
	short tmp = ADCW;
	int voltage = tmp;
	voltage *= 5;
	if (voltage > 5000){
		voltage = 5000;
	}
	return voltage;
}

unsigned char read_char_from_pc(){
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void print_new_line(){
	usart_prints( "\n\r");
}

// command digits key
// invalid command
// invalid command --> 0x00
// M --> 0x01
// S:a,n,t --> 0x02
// R:a,n --> 0x03
// E:a,n,t,d --> 0x04
void interpret_command(const int command_code, const char * command_string){
	switch (command_code){
		case 1: ;
			int adc_output;
			adc_output = read_adc();
			char adc_buff[9];
			sprintf(adc_buff, "v=%d.%d V", adc_output/1000,adc_output%1000 );
			print_new_line();
			usart_prints(adc_buff);
			print_new_line();

			break;
		case 2:
			break;
		case 3:
			break;
		case 4 :
			break;	
	}

}

// command digits key
// invalid command
// invalid command --> 0x00
// M --> 0x01
// S:a,n,t --> 0x02
// R:a,n --> 0x03
// E:a,n,t,d --> 0x04
unsigned int read_command(char * command_array, size_t arr_len){
	print_new_line();
	usart_prints("Enter a command $> ");
	// reset command_array
	for(int i = 0; i < arr_len; ++i){
		command_array[i] = " ";
	}

	unsigned char first_char = " ";
	first_char = read_char_from_pc();

	unsigned short max_command_length = 0x00;
	unsigned int ret_code = 0x00;

	switch (first_char) {
		case 'M': 
			command_array[0] = first_char;
			max_command_length = 0x00;
			ret_code = 0x01;
			break;

		case 'S':
			command_array[0] = first_char;
			max_command_length = 0x07;
			ret_code = 0x02;
			break;

		case 'R':
			command_array[0] = first_char;
			max_command_length = 0x05;
			ret_code = 0x03;
			break;

		case 'E':
			command_array[0] = first_char;
			max_command_length = 0x09;
			ret_code = 0x04;
			break;

		default:
			print_new_line();
			usart_prints("Error: Invalid Command!!");
			print_new_line();
	}

	for (int i = 1; i < max_command_length; ++i) {
		command_array[i] = read_char_from_pc();
	}

	return ret_code;
}

void store_EEPROM(char addr){
	cli();
	
	//int data = read_adc();
	int data = 5000;

	while(EECR & (1<<EEPE)); // Wait until EEPE is zero
	EEARH = 0x00; // Writing the high address location
	EEARL = 0x00; // Writing the high address location
	EEDR = 0x07;
	EECR &= ~(1<<EEPM0);
	EECR &= ~(1<<EEPM1);    // Clear EEPM1 and 0 (4th + 5th)
	asm volatile(
		"cbi EECR,1" "\n\t"
		"sbi EECR,2" "\n\t"
		"sbi EECR,1" "\n\t"
	::);

	sei();
}

char retrieve_EEPROM(){}

// OLD CODE --> reverse string
// const char * sdata = "Enter 4 characters to reverse:"; // String in SRAM
//		int i = 0;
//		while (i < 4){
//			input = read_char_from_pc();
//			inputstring[i] = input;
//			i++;
//		}
//		strrev(inputstring);
//		print_new_line();
//		usart_prints(inputstring);