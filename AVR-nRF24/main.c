/*
 * AVR-nRF24.c
 *
 * Created: 4/30/2021 3:04:23 PM
 *
 * author: balinth0rvath 
 */ 

 #define INIT_SETUP
 #undef TEST_BLINK 
 #undef TEST_ONE_PACKET 
 #define TEST_LONGLOOP
 #undef TEST_SENSOR
 

#include "common.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "nrf24.h"
#include "ds18b20.h"


void send_response_with_head(void)
{					
	char payload[4] = {};
	nrf24_payload_buffer_item_t p;
	nrf24_get_buffer_head(&p);
	payload[0] = p.id;
	payload[1] = p.source_address;
	payload[2] = p.value;
	payload[3] = p.aux;
	send_response(payload);
	
}

void send_response_with_temperature(void)
{  
  char payload[4] = {};
  payload[0] = 0;
  payload[1] = 0;
  payload[2] = ds18b20_read_temperature() >> 4;
  payload[3] = 0;
  send_response(payload);
  
}

void send_response(char* payload)
{
  uint8_t status;
  nrf24_transmit_packet(payload, &status);
  _delay_ms(100);
  
  nrf24_transmit_packet(payload, &status);
  _delay_ms(100);
  
}

void send_dummy_packet(void)
{	
	uint8_t status;			
	char payload[4] = {};
	payload[0] = 54;
	payload[1] = 55;
	payload[2] = 53;
	payload[3] = 54;
	nrf24_transmit_packet(payload, &status);	
	_delay_ms(100);	
		
	nrf24_transmit_packet(payload, &status);	
	_delay_ms(100);	
	
}

static void blink(uint8_t number, uint8_t fast)
{
	int i=0;
	for(i=0;i<number;i++)
	{
		PORTD ^= (1 << PD0);
		if (fast)
    {
			_delay_ms(50);		
    }
		else
    {
			_delay_ms(300);		
    }
	}	
}

int main(void)
{	
  volatile int ret;	
	DDRD|=( 1 << PD0);	
  PORTD &= ~(1 << PD0);

#ifdef INIT_SETUP
	//blink(2,0);
	nrf24_init(0);	
	ret = nrf24_check_device();	
  
	if (ret!=0)
	{
		blink(10,1);
	}
	
	uint8_t address[5] = {0x10, 0x20, 0x30, 0x40, 0x50};
	
	nrf24_set_rx_address_p0(address);
	nrf24_set_tx_address(address);	
	nrf24_set_receiver();	
	
	EICRA &= ~(1 << ISC00 | 1 << ISC01);
	EIMSK |= (1 << INT0);
	sei();	
#endif // INIT_SETUP	

#ifdef TEST_BLINK
  _delay_ms(1000);	
  int i = 20;
  while(i--)
  {
    PORTD ^= (1 << PD0);
    _delay_ms(200);	
  }
  while(1);
#endif // TEST_BLINK

#ifdef TEST_ONE_PACKET
	nrf24_set_transmitter();
	_delay_ms(10);
	send_dummy_packet();
	_delay_ms(10);
	while(1);
	
#endif //ONE_PACKET

#ifdef TEST_LONGLOOP
	nrf24_set_receiver();	
	while(1)	
	{		
		_delay_ms(10);
		if (nrf4_message_received())			
		{			      
			nrf24_set_transmitter();
			send_response_with_head();
			
			blink(8,1);
			nrf24_set_receiver();	
		}
	}	
#endif //TEST_LONGLOOP

#ifdef TEST_SENSOR

  ds18b20_init_driver();
  nrf24_set_receiver();
  while(1)
  {
    _delay_ms(10);
    if (nrf4_message_received())
    {
      nrf24_set_transmitter();
      send_response_with_temperature();
      
      blink(8,1);
      nrf24_set_receiver();
    }
  }  
#endif // TEST_SENSOR
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


