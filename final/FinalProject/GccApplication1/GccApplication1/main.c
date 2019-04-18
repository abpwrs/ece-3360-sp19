/*
 * GccApplication1.c
 *
 * Created: 4/18/2019 10:09:44 AM
 * Author : bmitchinson
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>




// GLOBAL



// Button States
// UP = 0x01
// DOWN = 0x00
 char prev_state = 0x00;
 char curr_state = 0x00;
 int  press_len = 0;


// timer that does a thing TBD
// interrupt HERE
// reset the timer


// button press interrupt 
// all of the logic here
name(INT0_vect){
	//if (PORTC & (1<<5)){
		//PORTC |= (0<<5);
	//} else {
		//PORTC |= (1<<5);
	//}
	PORTC |= 1 << 5;
	_delay_ms(1000);
	PORTC |= 0 << 5;

}


// prototypes



int main(void)
{
	DDRC |= 1 << 5;
	EICRA |= (1<<ISC00);
	EIMSK |= (1<<INT0);
	sei();
    /* Replace with your application code */
    while (1) 
    {
    }
}

