/*
 * AVR-nRF24.c
 *
 * Created: 4/30/2021 3:04:23 PM
 */ 

 

#include "common.h"
#include <avr/io.h>
#include <util/delay.h>			// add this to use the delay function
#include "nrf24.h"

void receiver(void)
{
    nrf24_init(NRF24_SET_RECEIVER);
    while (1) {					// begin infinite loop
	    nrf24_receive_poll();
	    _delay_ms(530);			// delay 1/2 second
    }

}

void transmitter(void)
{
	char payload[4] = {53,54,55,56};
	DDRD |= (1 << PD0);
	DDRD |= (1 << PD1);
	DDRD |= (1 << PD2);
	PORTD |= (1 << PD0);
	PORTD |= (1 << PD1);
	PORTD |= (1 << PD2);
	_delay_ms(1000);
	PORTD &= ~(1 << PD0);
	PORTD &= ~(1 << PD1);
	PORTD &= ~(1 << PD2);



	uint8_t status;
	int wait = 40;
	nrf24_init(NRF24_SET_TRANSMITTER);	
	nrf24_transmit_packet(payload, &status, &wait);

	if (!wait) // Wait countdown expired RED 100ms
	{
		while(1) 
		{
			PORTD ^= (1 << PD2);
			_delay_ms(100);
		}
	}

	if (status & 0x10)
	{
		while(1) // Maximum number of TX retransmits interrupt RED 500ms
		{
			PORTD ^= (1 << PD2);
			_delay_ms(500);
		}
	}
	if (status & 0x20) // Data Sent TX FIFO success GREEN 500ms
	{

		while(1) {
		
			PORTD ^= (1 << PD0);
			if (status != 46)
				_delay_ms(500);
			else;			
				_delay_ms(50);
		}
	}
	PORTD |= (1 << PD2); // Error continuous RED
	while(1);
}

int main(void)
{	_delay_ms(1000);
	//receiver();	
	transmitter();
}

