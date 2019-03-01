//simple testing program for sending and reading a char through the serial port

//just a reminder for myself(Ian):
//use 
//sudo picocom --baud 9600 /dev/ttyUSB0 
//to communicate with the serial port

#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#define FOSC 7372800
#define BAUD 9600
#define the_ubrr (FOSC/16/BAUD-1)

/*
	serial_init - Initialize the USART port
*/
void serial_init () {
	UBRR0 = the_ubrr ; // Set baud rate
	UCSR0B |= (1 << TXEN0 ); // Turn on transmitter
	UCSR0B |= (1 << RXEN0 ); // Turn on receiver
	UCSR0C = (3 << UCSZ00 ); // Set for async . operation , no parity ,
	// one stop bit , 8 data bits
}
/*
	serial_out - Output a byte to the USART0 port
*/
void serial_out ( char ch )
{
	while (( UCSR0A & (1 << UDRE0 )) == 0);
	UDR0 = ch ;
}

void serial_string_out ( char* ch )
{
	int i = 0;
	for(i = 0; i < strlen(ch); i++)
	{
		_delay_ms(1);
		serial_out(ch[i]);
	}
}

/*
	serial_in - Read a byte from the USART0 and return it
*/
char serial_in ()
{
	//Potential problem, could lock the program 
	while ( !( UCSR0A & (1 << RXC0 )) );
	return UDR0 ;
}

int main(void)
{
    
	while(1)
	{
		
	}
}
