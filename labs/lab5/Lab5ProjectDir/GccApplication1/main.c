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

// prints
void usart_prints(const char *);
void usart_printf();
void print_new_line();
void print_single_line_message(const char *);
// usart io
unsigned char read_char_from_pc();
void usart_init();
// using the adc
void adc_init();
int read_adc();
// command interp
unsigned int read_command(char *, size_t);
void interpret_command(const char *);

void execute_M(); // M don't care about your args
void execute_R(int *, int *);
void execute_S(int *, int *, int *);
void execute_E(int *, int *, int *, int *);
char parse_args(const char *, int *, int *, int *, int *);

// our EEPROM methods (avoiding interrupts)
void my_eeprom_write_word(uint16_t, uint16_t);
uint16_t my_eeprom_read_word(uint16_t);



int main(void)
{
	usart_init();
	adc_init();
	const size_t arr_len = 9;  // max length of a command
	char inputstring[arr_len]; // input string to hold commands
	unsigned int command_code;

	while (1)
	{
		command_code = read_command(inputstring, arr_len);
		if (command_code != 0x00)
		{
			interpret_command(inputstring);
		}
	}
}

// Set Asynchronous Normal Mode and configure BAUD Rate @ 9600 -- TODO: what is this in reference to?

// USART Functions
// ///////////////////////////////////////////////////////////////////
void usart_init()
{
	UCSR0A = UCSR0A & ~(0x02);			  // Set the mode to set "Async Normal Mode" (Slide 45 SerialComm)
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // set to transmit and receive
	UBRR0 = 0x33;						  // UBRR0 = [8000000 / 16(9600)] - 1 = 51.083 (51?)
}

void usart_prints(const char *sdata)
{
	while (*sdata)
	{
		// Wait for UDRE0 to become set (==1), which indicates
		// the UDR0 is empty and can receive the next character (Slide 46, Serial Comm)
		while (!(UCSR0A & (1 << UDRE0)))
			; // Option A
		//while (!(UCSR0A & (1<<TXC0))); // Option B
		UDR0 = *(sdata++);
	}
}
// ///////////////////////////////////////////////////////////////////

// ADC Functions
// ///////////////////////////////////////////////////////////////////
// Configure ADC
void adc_init()
{
	ADMUX = ADMUX | (1 << REFS0); // Configure ADC Reference
	ADCSRA = (1 << ADEN);
}

int read_adc()
{
	ADCSRA = ADCSRA | (1 << ADSC);
	while (ADCSRA & (1 << ADIF))
		;
	short tmp = ADCW;
	int voltage = tmp;
	voltage *= 5;
	if (voltage > 5000)
	{
		voltage = 5000;
	}
	return voltage;
}
// ///////////////////////////////////////////////////////////////////

// command digits key
// invalid command
// invalid command --> 0x00
// M --> 0x01
// S:a,n,t --> 0x02
// R:a,n --> 0x03
// E:a,n,t,d --> 0x04
// Command Logic
// ///////////////////////////////////////////////////////////////////
void interpret_command(const char *command_string)
{
	int a, n, t, d;
	char failure;
	failure = parse_args(command_string, &a, &n, &t, &d);
	if (failure == 0x01)
	{
		print_single_line_message("Failure to Parse!");
	}
	else
	{
		switch (command_string[0])
		{
		case 'M':
			execute_M();
			break;
		case 'S':
			execute_S(&a, &n, &t);
			break;
		case 'R':
			execute_R(&a, &n);
			break;
		case 'E':
			execute_E(&a, &n, &t, &d);
			break;
		}
	}
}

// parse args --> the return code can be passed by ref to reduce mem usage
// ///////////////////////////////////////////////////////////////////
char parse_args(const char *command, int *a, int *n, int *t, int *d)
{
	if (command[0]=='M'){
		return 0x00;
	}

	*a = atoi(command[2]);
	*n = atoi(command[4]);

	// repeated range checks should be a function
	if (*a < 0 || *a > 510){
		print_single_line_message("0 =< a =< 510");
		return 0x01;
	}

	if (*n < 1 || *n > 20){
		print_single_line_message("1 =< n =< 20");
		return 0x01;
	}

	if (command[0] != 'R')
	{
		t = atoi(command[6]);
		if (*t < 1 || *t > 10){
		print_single_line_message("1 =< t =< 10");
		return 0x01;
	}
	}
	else
	{
		*t = 0;
	}
	if (command[0] == 'E')
	{
		*d = atoi(command[8]);
		if (*d < 0 || *d > 1){
			print_single_line_message("0 =< t =< 1");
			return 0x01;
		}
	}
	else
	{
		*d = 0;
	}
	return 0x00;
}
// ///////////////////////////////////////////////////////////////////

// specific functions for each sub-command
// M
// ///////////////////////////////////////////////////////////////////
void execute_M()
{
	int adc_output;
	adc_output = read_adc();
	char adc_buff[9];
	sprintf(adc_buff, "v=%d.%d V", adc_output / 1000, adc_output % 1000);
	print_new_line();
	usart_prints(adc_buff);
	print_new_line();
	return 0x00; // hard coded return no failure
}
// ///////////////////////////////////////////////////////////////////

// S
// ///////////////////////////////////////////////////////////////////
// TODO: implement S functionality
void execute_S(int *a, int *n, int *t)
{
}

// ///////////////////////////////////////////////////////////////////

// R
// ///////////////////////////////////////////////////////////////////
// TODO: implement R functionality
void execute_R(int *a, int *n)
{
}
// ///////////////////////////////////////////////////////////////////

// E
// ///////////////////////////////////////////////////////////////////
// TODO: implement E functionality
void execute_E(int *a, int *n, int *t, int *d)
{
}
// ///////////////////////////////////////////////////////////////////

// our eeprom interaction methods (effectively just avoiding interrupts)
// ///////////////////////////////////////////////////////////////////
void my_eeprom_write_word(uint16_t addr, uint16_t value)
{
	cli();
	eeprom_write_word(addr, value);
	sei();
}

uint16_t my_eeprom_read_word(uint16_t addr)
{
	uint16_t tmp;
	cli();
	tmp = eeprom_read_word(addr);
	sei();
	return tmp;
}
// ///////////////////////////////////////////////////////////////////

// command digits key
// invalid command
// invalid command --> 0x00
// M --> 0x01
// S:a,n,t --> 0x02
// R:a,n --> 0x03
// E:a,n,t,d --> 0x04
// ///////////////////////////////////////////////////////////////////
unsigned int read_command(char *command_array, size_t arr_len)
{
	print_new_line();
	usart_prints("Enter a command $> ");
	const int CODE_TO_LENGTH[5] = {0, 0, 11, 8, 13}; // IF WE RUN INTO MEM TROUBLE REMOVE THIS

	// reset command_array
	for (int i = 0; i < arr_len; ++i)
	{
		command_array[i] = " ";
	}

	unsigned char first_char = " ";
	first_char = read_char_from_pc();

	unsigned short max_command_length = 0x00;
	unsigned int ret_code = 0x00;

	switch (first_char)
	{
	case 'M':
		command_array[0] = first_char;
		ret_code = 1;
		break;

	case 'S':
		command_array[0] = first_char;
		ret_code = 2;
		break;

	case 'R':
		command_array[0] = first_char;
		ret_code = 3;
		break;

	case 'E':
		command_array[0] = first_char;
		ret_code = 4;
		break;

	default:
		print_single_line_message("Error: Invalid Command!!");
	}

	max_command_length = CODE_TO_LENGTH[ret_code];
	unsigned int curr_read = 1;
	unsigned char done = 0x00;
	char curr_char;
	while(curr_read < max_command_length && done != 0x01){
		curr_char = read_char_from_pc();
		if (curr_char == '\r'){

		} else if (curr_char == ','){

		}
		
	}



	for (int i = 1; i < max_command_length; ++i)
	{
		command_array[i] = read_char_from_pc();
	}

	return ret_code;

} // Reading from serial input
// ///////////////////////////////////////////////////////////////////
unsigned char read_char_from_pc()
{
	while (!(UCSR0A & (1 << RXC0)))
		;
	return UDR0;
}
// ///////////////////////////////////////////////////////////////////

// utility prints
// ///////////////////////////////////////////////////////////////////
void print_new_line()
{
	usart_prints("\n\r");
}

void print_single_line_message(const char *message)
{
	print_new_line();
	usart_prints(message);
	print_new_line();
}

// ///////////////////////////////////////////////////////////////////

// old code temporarily kept for reference
// ///////////////////////////////////////////////////////////////////
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

// EEPROM writing reading and writing
// eeprom_write_word(0x00, 4387);
// int result = eeprom_read_word(0x00);
// char thingy[20];
// sprintf(thingy, "Got: %d", result);
// usart_prints(thingy);
// print_new_line();