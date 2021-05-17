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

void send_response(void)
{	
	uint8_t status;			
	char payload[4] = {};
	nrf24_payload_buffer_item_t p;
	nrf24_get_buffer_head(&p);
	payload[0] = p.id;
	payload[1] = p.source_address;
	payload[2] = p.value;
	payload[3] = p.aux;
	nrf24_transmit_packet(payload, &status);	
	_delay_ms(100);	
		
	nrf24_transmit_packet(payload, &status);	
	_delay_ms(100);	
	
}

int main(void)
{	
	DDRD|=( 1 << PD0);	
	nrf24_init(0);	
	
	nrf24_set_receiver();	

	EICRA &= ~(1 << ISC00 | 1 << ISC01);
	EIMSK |= (1 << INT0);
	sei();	
	
	while(1)	
	{		
		_delay_ms(100);
		if (nrf4_message_received())			
		{
			_delay_ms(100);
			nrf24_set_transmitter();
			send_response();
			_delay_ms(100);
			nrf24_set_receiver();	
		}
	}	
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


