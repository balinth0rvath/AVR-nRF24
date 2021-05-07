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

void receiver(void)
{
    nrf24_init(NRF24_SET_RECEIVER);
	EICRA &= ~(1 << ISC00 | 1 << ISC01);
	EIMSK |= (1 << INT0);
	sei();     
}

void transmitter(void)
{	
	
	uint8_t status;
	int wait = 40;
	nrf24_init(NRF24_SET_RECEIVER);			
	char payload[4] = {53,54,55,49};	
	nrf24_transmit_packet(payload, &status, &wait);
	payload[3] = 48;	
	_delay_ms(500);
	nrf24_transmit_packet(payload, &status, &wait);
	_delay_ms(500);
	if (status & 0x20) // Data Sent TX FIFO success GREEN 500ms
	{
		while(1);
	}	
}

int main(void)
{	
	if (NRF24_SET_RECEIVER)
	{
		receiver();			
	}
	else
	{
		transmitter();
	}	
	while(1);
}

ISR (INT0_vect)
{	
	nrf24_receive();
}


