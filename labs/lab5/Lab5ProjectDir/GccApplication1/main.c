//////////////////////////////////////////////////////////////////////
// Assembly language file for Lab 5 in ECE:3360 (Embedded Systems)
// Spring 2019, The University of Iowa.
//
// Desc: Lab 5 Analog Data Logging System
//
// Authors: B. Mitchinson, A. Powers
//////////////////////////////////////////////////////////////////////

#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz -> CProgramming Notes, Slide 10
#endif 

#include <avr/io.h>
#include <util/delay.h>

void setbaud();
void transmitenable();
void sendhi();
void transmitdisable();

unsigned char tmp;	 // Compiler puts this in SRAM

int main(void)
{
    // unsigned char c;		 // Compiler puts this in SRAM
	// unsigned char tmp;	 // Compiler puts this in SRAM
	// DDRC = 0x20;			 // PORTC,5 Output (sbi DDRC, 5)
	
	setbaud();
	transmitenable();
	sendhi();
	//transmitdisable();
	
    while (1) 
    {
		// TODO: Polling logic to switch between send + receive	
		
		/////////////////////////////////////////////////////////////
		//tmp = PORTC;		 // Get PortC
		//tmp = tmp | 0x20;  // Set bit 5
		//PORTC = tmp;		 // Update PortC (sbi PORTC,5)
		//_delay_ms(300.0);	 // Delays do weird stuff based on compile method,
							 //    so be careful. (Slide 17 + 27, CProgramming)

		//tmp = PORTC;		 // Get PortC
		//tmp = tmp & ~(0x20); // Clear bit 5
		//PORTC = tmp;		 // Update PORTC,5 (cbi PORTC,5)
		//_delay_ms(100.0);
    }
}

// Set Asynchronous Normal Mode and configure BAUD Rate @ 9600
void setbaud(){
	tmp = UCSR0A;		  // Get mode of operation
	tmp = tmp & ~(0x02);  // Clear bit 1 to set "Async Normal Mode" (Slide 45 SerialComm)
	UCSR0A = tmp;		  // Set the mode
	tmp = 0x33;			  // UBRR0 = [8000000 / 16(9600)] - 1 = 51.083 (51?)
	UBRR0 = tmp;
}

// Enable transmit TXEN0 bit in UCSR0b
void transmitenable(){
	tmp = UCSR0B;		 
	tmp = tmp |	0x08;	  // Set TXEN0 (Bit 3) of UCSR0B
	UCSR0B = tmp;
}

// Disable transmit TXEN0 bit in UCSR0b
void transmitdisable(){
	tmp = UCSR0B;
	tmp = tmp &	~(0x08);  // Clear TXEN0 (Bit 3) of UCSR0B
	UCSR0B = tmp;
}

// Load one character into UDR0 for testing transfer
void sendhi(){
	tmp = 0x0A; // Newline
	UDR0 = tmp;
	_delay_ms(20);
	tmp = 0x0D; // Carriage Return
	UDR0 = tmp;
	_delay_ms(20);
	tmp = 0x48; // H
	UDR0 = tmp;
	_delay_ms(20);
	tmp = 0x69; // i
	UDR0 = tmp;
	_delay_ms(20);
	tmp = 0x21; // !
	UDR0 = tmp;
	_delay_ms(20);
}