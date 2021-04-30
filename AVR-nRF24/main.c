/*
 * AVR-nRF24.c
 *
 * Created: 4/30/2021 3:04:23 PM
 */ 

 

#include "common.h"
#include <avr/io.h>
#include <util/delay.h>			// add this to use the delay function
#include "nrf24.h"

int main(void)
{
    //DDRD &= ~(1 << PD2);
    //PORTD |= (1 << PD2);
    DDRC |= (1 << PC1);			// set Port C pin PC1 for output
    DDRC |= (1 << PC2);			// set Port C pin PC2 for output
    DDRC |= (1 << PC3);			// set Port C pin PC3 for output
    DDRC |= (1 << PC4);			// set Port C pin PC4 for output
    DDRC |= (1 << PC5);			// set Port C pin PC5 for output
    
    while (1) {					// begin infinite loop
	    //if (!(PIND & (1 << PD2)))
	    PORTC ^= (1 << PC1);	// flip state of LED on PC1
	    PORTC ^= (1 << PC2);	// flip state of LED on PC2
	    PORTC ^= (1 << PC3);	// flip state of LED on PC3
	    PORTC ^= (1 << PC4);	// flip state of LED on PC4
	    PORTC ^= (1 << PC5);	// flip state of LED on PC5

	    _delay_ms(530);			// delay 1/2 second
    }

}

