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

static int blink(uint8_t number, uint8_t fast)
{
	int i=0;
	for(i=0;i<number;i++)
	{
		PORTD ^= (1 << PD0);
		if (fast)
			_delay_ms(10);		
		else
			_delay_ms(300);		
	}
	PORTD &= ~(1 << PD0);
}

int main(void)
{	
	volatile int ret;
	DDRD|=( 1 << PD0);	
	PORTD &= ~(1 << PD0);
	blink(2,0);
	nrf24_init(0);	
	ret = nrf24_check_device();
	if (ret!=0)
	{
		blink(10,0);
	}

	uint8_t rx_address[5] = {0xe7, 0xe7, 0xe7, 0xe7, 0xe7};
	uint8_t tx_address[5] = {0xe7, 0xe7, 0xe7, 0xe7, 0xe7};

	volatile uint8_t rx_address2[5] = {};
	volatile uint8_t tx_address2[5] = {};
	
	//nrf24_set_rx_address_p0(rx_address);
	//nrf24_set_tx_address(tx_address);	
	nrf24_set_receiver();	
	
	EICRA &= ~(1 << ISC00 | 1 << ISC01);
	EIMSK |= (1 << INT0);
	sei();	
	
	while(1)	
	{		
		_delay_ms(10);
		//nrf24_get_rx_address_p0(rx_address2);
		//nrf24_get_tx_address(tx_address2);
		if (nrf4_message_received())			
		{			
			nrf24_set_transmitter();
			send_response();
			PORTD |= (1 << PD0);
			//_delay_ms(100);
			PORTD &= ~(1 << PD0);
			blink(5,1);
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


