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
	
    nrf24_init();
    
    while (1) {					// begin infinite loop
	    
	    NRF24_PORT ^= (1 << NRF24_GPIO_SCLK);	

	    _delay_ms(530);			// delay 1/2 second
    }

}

