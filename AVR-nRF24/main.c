/*
 * AVR-nRF24.c
 *
 * Created: 4/30/2021 3:04:23 PM
 */ 

#include "common.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "nrf24.h"

void transmitter(void)
{	
	uint8_t status;			
	char payload[4] = {53,54,55,49};	
	nrf24_transmit_packet(payload, &status);
	payload[3] = 48;	
	_delay_ms(500);	
		
	nrf24_transmit_packet(payload, &status);	
	_delay_ms(500);	
	
}

int main(void)
{	
	DDRD|=( 1 << PD0);	
	nrf24_init(0);	
	//nrf24_set_receiver();
	nrf24_set_transmitter();

	EICRA &= ~(1 << ISC00 | 1 << ISC01);
	EIMSK |= (1 << INT0);
	sei();
	transmitter();
	/*
	while(1);
	{		
	}
	*/
}

ISR (INT0_vect)
{	
	volatile uint8_t ret = nrf24_get_status();	
	if (ret & 0x40)
	{
		nrf24_receive_irq();
	} else if (ret & 0x20)
	{
		nrf24_transmit_irq();			
	}	
}


