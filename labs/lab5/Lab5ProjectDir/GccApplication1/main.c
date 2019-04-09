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
#include <i2cmaster.h>
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
int check_bounds(int);
// command interp
unsigned int read_command(char *, size_t);
void interpret_command(const char *);

void execute_M(); // M don't care about your args
void execute_R(int *);
void execute_S(int *);
void execute_E(int *);
void parse_args(const char *, int *);

// INPUT VALIDATION
char validate_input(int *);
char check_param(int, int, int, char *);

// ISR inits
void timer1_init();

// our EEPROM methods (avoiding interrupts)
void my_eeprom_write_word(uint16_t, uint16_t);
uint16_t my_eeprom_read_word(uint16_t);

// Global timer variables
volatile int T_END = 0;
volatile int T_CURRENT = 0;
volatile int N_END = 0;
volatile int N_CURRENT = 0;
volatile char STORE_IN_PROG = 0x00;

// Timer configuration
void timer1_init(){
	//TCCR1A |= ();
	TCCR1B |= (1<<CS12);
}

ISR(TIMER1_OVF_vect) {
	//ISR: Called every second
	//* Updates T_CURRENT to compare to T_END
	//* Upon T_END = T_CURRENT
	//** Take the sample -> store it (address++)
	//** reset T_END, N_CURRENT++
	//** When N_CURRENT = N_END, turn off timer
}

int main(void)
{
	usart_init();
	adc_init();
	const size_t arr_len = 14;  // max length of a command
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
int check_bounds(int voltage){
	if (voltage < 0){
		voltage = 0;
	} else if (voltage > 5000){
		voltage = 0;
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
	int param_arr[4];
	char failure;
	parse_args(command_string, param_arr);
	failure = validate_input(param_arr);
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
			execute_S(param_arr);
			break;
		case 'R':
			execute_R(param_arr);
			break;
		case 'E':
			execute_E(param_arr);
			break;
		}
	}
}

// INPUT VALIDATION:
char validate_input(int * params){
	char ret_val = 0x00;
	ret_val |= check_param(params[0], 0, 510, "0 =< a =< 510");
	ret_val |= check_param(params[1], 1, 20, "1 =< n =< 20");
	ret_val |= check_param(params[2], 1, 10, "1 =< t =< 10");
	ret_val |= check_param(params[3], 1, 1, "0 =< d =< 1");
	if (params[0] + (2*params[1]) > 510){
		ret_val = 0x01;
		print_single_line_message("a + (2 * n) <= 510");
	}
	return ret_val;
}

char check_param(int value, int min_v, int max_v, char * message){
	if (value < min_v || value > max_v){
		print_single_line_message(message);
		return 0x01;
	}
	return 0x00;
}

// parse args --> the return code can be passed by ref to reduce mem usage
// ///////////////////////////////////////////////////////////////////
void parse_args(const char *command, int *arr)
{
	for (int i = 0; i<4; ++i){
		arr[i] = 1;
	}
	if(command[0] != 'M'){
		unsigned int param_count = 0;
		unsigned int command_index = 1;
		char param[3];
		unsigned int param_index = 0;

		do {
			command_index += 1;

			if(command[command_index] == ',' || command[command_index] == '\0'){
				// save param
				arr[param_count] = atoi(param);
				param_count += 1;
				// reset param
				param_index=0;
				for(int i=0; i<3; ++i){
					param[i] = '\0';
				}
			} else{
				// add to param buffer
				param[param_index] = command[command_index];
				param_index += 1;
			}
		} while (command[command_index] != '\0');
	}
}

// ///////////////////////////////////////////////////////////////////

// specific functions for each sub-command
// M
// ///////////////////////////////////////////////////////////////////
void execute_M()
{
	int adc_output;
	adc_output = check_bounds(read_adc());
	char adc_buff[10];
	sprintf(adc_buff, "v=%d.%d V", adc_output / 1000, adc_output % 1000);
	print_single_line_message(adc_buff);
}
// ///////////////////////////////////////////////////////////////////

// S
// ///////////////////////////////////////////////////////////////////
// TODO: implement S functionality
void execute_S(int *params)
{

	// blocking store
	int adc_val;
	for(int current_n = 0; current_n < params[1]; ++current_n){
		adc_val = check_bounds(read_adc());
		my_eeprom_write_word(params[0] + (current_n * 2), adc_val);
		// diagnostic print
		char adc_buff[25];
		sprintf(adc_buff, "Storing v=%d.%d V at %d", adc_val / 1000, adc_val % 1000, params[0] + (current_n * 2));
		print_single_line_message(adc_buff);
		switch(params[2]){
		case 1:
		_delay_ms(1000);
		break;
		case 2:
		_delay_ms(2000);
		break;
		case 3:
		_delay_ms(3000);
		break;
		case 4:
		_delay_ms(4000);
		break;
		case 5:
		_delay_ms(5000);
		break;
		case 6:
		_delay_ms(6000);
		break;
		case 7:
		_delay_ms(7000);
		break;
		case 8:
		_delay_ms(8000);
		break;
		case 9:
		_delay_ms(9000);
		break;
		case 10:
		_delay_ms(10000);
		break;
		}
		

	}
}

// ///////////////////////////////////////////////////////////////////

// R
// ///////////////////////////////////////////////////////////////////
void execute_R(int *params)
{
	int adc_val;
	for(int current_n = 0; current_n < params[1]; ++current_n){
		adc_val = check_bounds(my_eeprom_read_word(params[0] + (current_n * 2)));
		char adc_buff[10];
		sprintf(adc_buff, "v=%d.%d V", adc_val / 1000, adc_val % 1000);
		print_single_line_message(adc_buff);
	}
}
// ///////////////////////////////////////////////////////////////////

// E
// ///////////////////////////////////////////////////////////////////
// TODO: implement E functionality
void execute_E(int *params)
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
		command_array[i] = "\0";
	}

	unsigned char first_char = " ";
	first_char = read_char_from_pc();

	unsigned short max_command_length = 0x00;
	unsigned int ret_code = 0x00;
	command_array[0] = first_char;

	switch (first_char)
	{
	case 'M':
		ret_code = 1;
		break;

	case 'S':
		ret_code = 2;
		break;

	case 'R':
		ret_code = 3;
		break;

	case 'E':
		ret_code = 4;
		break;

	default:
		print_single_line_message("Error: Invalid Command!!");
	}
	max_command_length = CODE_TO_LENGTH[ret_code];
	char curr_char;
	unsigned int curr_read = 1;
	while(curr_read < max_command_length && curr_char != '\r'){
		curr_char = read_char_from_pc();
		if (curr_char != '\r')
		{
			command_array[curr_read] = curr_char;
		}
		curr_read += 1;
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
