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

int main(void)
{
    unsigned char tmp;		 // Compiler puts this in SRAM
	
	DDRC = 0x20;			 // PORTC,5 Output (sbi DDRC, 5)
    while (1) 
    {
		tmp = PORTC;		 // Get PortC
		tmp = tmp | 0x20;    // Set bit 5
		PORTC = tmp;		 // Update PortC (sbi PORTC,5)
		_delay_ms(300.0);	 // Delays do weird stuff based on compile method,
							 //    so be careful. (Slide 17 + 27, CProgramming)
		
		tmp = PORTC;		 // Get PortC
		tmp = tmp & ~(0x20); // Clear bit 5
		PORTC = tmp;		 // Update PORTC,5 (cbi PORTC,5)
		_delay_ms(100.0);
    }
}

