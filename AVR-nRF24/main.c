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

	uint8_t status;
	int wait = 4000;
	nrf24_init(NRF24_SET_TRANSMITTER);
	nrf24_transmit_packet(payload, &status, &wait);

	if (!wait)
	{
		while(1);
	}

	if (status & 0x10)
	{
		while(1);
	}
	if (status & 0x20)
	{
		while(1);
	}
	while(1);
}

int main(void)
{	
	receiver();
	transmitter();
}

