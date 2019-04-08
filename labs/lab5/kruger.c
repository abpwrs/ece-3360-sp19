//  Sample code for using USART, WINAVR
//
//  A. Kruger, April 2010
//
//  Transmission is polled, but reception is interrupt driven.  In
//  applications that interact with a user via a RS232 link and a PC,
//  polled data transmission from controller to PC is not a limitation.
//
//  The code below assumes an AVR with USART  TX and RX pins on PD0 and
//  PD1 respectively.  For example, ATmega88PA.  Change usart_init to
//  match your processor.   


#define F_CPU 8000000L        // This should match the processor speed
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>     //  Routine for FLASH (program memory)
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#define BAUD_RATE 9600        // Baud rate. The usart_int routine
                              // uses this and then sets 8N1         

// Variables and #define for the RX ring buffer.

#define RX_BUFFER_SIZE 64 
unsigned char rx_buffer[RX_BUFFER_SIZE];
volatile unsigned char rx_buffer_head;
volatile unsigned char rx_buffer_tail;

// Funtion prototypes.

unsigned char uart_buffer_empty(void);
void usart_prints(const char *ptr);
void usart_printf(const char *ptr);
void usart_init(void);
void getDate(int *yy,int *mm, int*dd);
void usart_putc(const char c);
unsigned char usart_getc(void);

// Sample strings in SRAM and Flash, used for examples.

const char sdata[] = "Hello World!\n";          // String in SRAM
const char fdata[] PROGMEM = "Flash Gordon\n";  // String in Flash

int main(void)
{
   unsigned char c;
   char str[25];
   int  yy,mm,dd;
   int i;

   sei();                  // Enable interrupts     
   
   usart_init();           // Initialize the USART
   usart_prints(sdata);    // Print a string from SRAM
   usart_printf(fdata);    // Print a string from FLASH

   // Get the date, make a formatted string, and then
   // send via the USART.   

   getDate(&yy,&mm,&dd);
   sprintf(str,"Date: %d/%d/%d\n",yy,mm,dd);
   usart_prints(str);

   // Read in the year part of a date.

   usart_prints("Please Enter Year (yyyy):");
   for (i=0;i<=4-1;i++){
      c = usart_getc();    // Get characterg
      usart_putc(c);       // Echo it back
      str[i] = c;
   }
   str[i] = '\0';

   // Convert the string to a number.

   sscanf(str,"%d",&yy);
   if ( yy < 2010) {
      sprintf(str,"\nInvalid year: %d\n",yy);
      usart_prints(str);
   }
   
   usart_prints("Please Enter Month (mm):"); 
   while(1)
      ;
   
   return(1);
}

void getDate(int *yy,int *mm, int*dd)
{
   // This is a stub with hardcoded dates. 
   // Your application will get the date from
   // the RTC chip.
   
   *yy = 2010;
   *mm = 4;
   *dd = 16;
}


ISR(USART_RX_vect)
{
   // UART receive interrupt handler.
   // To do: check and warn if buffer overflows.
   
   char c = UDR0; 
   rx_buffer[rx_buffer_head] = c;
   if (rx_buffer_head == RX_BUFFER_SIZE - 1)
      rx_buffer_head = 0;
   else
      rx_buffer_head++;
}

void usart_init(void)
{
   // Configures the USART for serial 8N1 with
   // the Baud rate controlled by a #define. 

   unsigned short s;
  
   // Set Baud rate, controlled with #define above.
  
   s = (double)F_CPU / (BAUD_RATE*16.0) - 1.0; 
   UBRR0H = (s & 0xFF00);
   UBRR0L = (s & 0x00FF);

   // Receive complete interrupt enable: RXCIE0
   // Receiver & Transmitter enable: RXEN0,TXEN0

   UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);

   // Along with UCSZ02 bit in UCSR0B, set 8 bits
   
   UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
 
   DDRD |= (1<< 1);         // PD0 is output (TX) 
   DDRD &= ~(1<< 0);        // PD1 is input (Rx)
  
   // Empty buffers
 
   rx_buffer_head = 0;
   rx_buffer_tail = 0;
}


void usart_printf(const char *ptr){

   // Send NULL-terminated data from FLASH.
   // Uses polling (and it blocks).

   char c;

   while(pgm_read_byte_near(ptr)) {
      c = pgm_read_byte_near(ptr++); 
      usart_putc(c);
   }
}

void usart_putc(const char c){

   // Send "c" via the USART.  Uses poling
   // (and it blocks). Wait for UDRE0 to become
   // set (=1), which indicates the UDR0 is empty
   // and can accept the next character.

   while (!(UCSR0A & (1<<UDRE0)))  
      ;
   UDR0 = c;
}

void usart_prints(const char *ptr){
   
   // Send NULL-terminated data from SRAM.
   // Uses polling (and it blocks).

   while(*ptr) {
      while (!( UCSR0A & (1<<UDRE0)))  
         ;
      UDR0 = *(ptr++);  
  }
}

unsigned char usart_getc(void) 
{

   // Get char from the receiver buffer.  This
   // function blocks until a character arrives.
    
   unsigned char c;
  
   // Wait for a character in the buffer.

   while (rx_buffer_tail == rx_buffer_head)
      ;  
  
   c = rx_buffer[rx_buffer_tail];
   if (rx_buffer_tail == RX_BUFFER_SIZE-1)
      rx_buffer_tail = 0;
   else
      rx_buffer_tail++;
    return c;
}

unsigned char uart_buffer_empty(void)
{
   // Returns TRUE if receive buffer is empty.
   
   return (rx_buffer_tail == rx_buffer_head);
}
