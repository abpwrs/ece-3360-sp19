/*
 * GccApplication1.c
 *
 * Created: 4/18/2019 10:09:44 AM
 * Author : bmitchinson
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "lcd.h"

// LCD #define statements
#define RS  5
#define E   3
#define D4  0
#define D5  1
#define D6  2
#define D7  3
#define LED_PORT  5
#define MORSE_ARR_LEN 6




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
ISR(INT0_vect){
    if (PORTC & (1<<5)){
        PORTC &= ~(1<<5);
    } else {
       PORTC |= (1<<5);
    }
}



// prototypes
// lcd initialization methods

void set_to_8_bit(void);
void set_to_4_bit(void);

// lcd strobe
void lcd_strobe(void);

// write to lcd
void lcd_write_char(char);
void lcd_write_str(char *, int);

// convert 6 integer array
char morse_to_ascii(int *, int);

int main(void)
{
    // LCD Data Direction Configuration
    DDRC |= (1<<D4);
    DDRC |= (1<<D5);
    DDRC |= (1<<D6);
    DDRC |= (1<<D7);
    DDRB |= (1<<RS);
    DDRB |= (1<<E);
    //my_lcd_init();
	lcd_init(LCD_DISP_ON_CURSOR_BLINK);
	lcd_puts("abc");
	lcd_putc('B');
	lcd_home();
	lcd_putc('Z');
	lcd_gotoxy(4,1);
	lcd_putc('Z');
    // set interrupt configurations
    DDRC |= 1 << 5;
    EICRA |= (1<<ISC00);
    EIMSK |= (1<<INT0);

	// lcd_write_str("TESTING", 7);
    // enable interrupts
    sei();

    // infinite loop
    while (1) 
    {
	    //PORTC |= 1 << 5;
	    //_delay_ms(1000);
	    //PORTC &= ~(1<<5);
	    //_delay_ms(1000);
		//lcd_write_char('A');
    }
}

void my_lcd_init(void){
// set to command mode
    PORTB &= ~(1<<RS);
// delay 10 ms
    _delay_ms(10);
// set to 8 bit
    send_to_lcd(0x03);
    send_to_lcd(0x03);
    send_to_lcd(0x03);
// set to 4 bit
    send_to_lcd(0x02);
    send_to_lcd(0x02);
// Two rows 5x7 characters
    send_to_lcd(0x08); 
// clear display
    send_to_lcd(0x00); 
// also clear display
    send_to_lcd(0x01); 
// display on, underline + blink off
    send_to_lcd(0x00); 
// also display on, underline + blink off 
    send_to_lcd(0x0c); 
// display shift off, address increment mode
    send_to_lcd(0x00); 
// also display shift off, address increment mode
    send_to_lcd(0x06); 
}

void lcd_write_char(char character){
	PORTB |= 1 << RS;
	character = ((character & 0x0F) << 4 | (character & 0xF0) >> 4);
	send_to_lcd(character);
	// swap nibbles
	character = ((character & 0x0F) << 4 | (character & 0xF0) >> 4);
	send_to_lcd(character);
	// swap nibbles back
}

void lcd_write_str(char * message, int length){
	for(int i; i<length; ++i){
		lcd_write_char(message[i]);
	}
}

// send command does not automatically set to command mode
void send_to_lcd(char command){
    // send the command
    PORTC = command;
    lcd_strobe();
    _delay_us(200);
}

void send_char(char message){

}


void lcd_strobe(void){
    PORTB &= ~(1<<E);
    _delay_us(200);
    PORTB |= (1<<E);
    _delay_us(200);
    PORTB &= ~(1<<E);
}


char morse_to_ascii(int * morse_arr, int used_len){
    switch (used_len){
        case 0:
            // error
            break;

        case 1: 
            // lookup table 1
            break;

        case 2: 
            // lookup table 2
            break;
        case 3: 
            // lookup table 3
            break;

        case 4: 
            // lookup table 4
            break;

        case 5: 
            // lookup table 5
            break;

        case 6: 
            // lookup table 6
            break;

        default:
            // error
            break;
    }

}

/*
ASCII, length 
45, 1
54, 1
41, 2
49, 2
4D, 2
4E, 2
44, 3
47, 3
4B, 3
4F, 3
52, 3
53, 3
55, 3
57, 3
42, 4
43, 4
46, 4
48, 4
4A, 4
4C, 4
50, 4
51, 4
56, 4
58, 4
59, 4
5A, 4
30, 5
31, 5
32, 5
33, 5
34, 5
35, 5
36, 5
37, 5
38, 5
39, 5
2E, 6
2C, 6
3F, 6
*/
