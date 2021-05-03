/*
* nrf24.c
*
* Created: 4/30/2021 4:40:35 PM
*/ 
#include "common.h"
#include <avr/io.h>
#include <util/delay.h>			// add this to use the delay function
#include "nrf24.h"
 
struct nrf24_dev_t {
	char payload_buffer[32];
} nrf24_dev;

void nrf24_init()
{
		
	struct nrf24_dev_t* nrf24_devp = &nrf24_dev;
	 
	int i;
	for(i=0;i<32;i++)
		nrf24_devp->payload_buffer[i]=i;

	NRF24_DDR |= (1 << NRF24_GPIO_CE);
	NRF24_DDR |= (1 << NRF24_GPIO_SCLK);
	NRF24_DDR |= (1 << NRF24_GPIO_MOSI);
	NRF24_DDR &= ~(1 << NRF24_GPIO_MISO); // input
	NRF24_DDR |= (1 << NRF24_GPIO_CSN);		
		
	NRF24_PORT &= ~(1 << NRF24_GPIO_CE);	 
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);
	NRF24_PORT &= ~(1 << NRF24_GPIO_SCLK);	
	NRF24_PORT &= ~(1 << NRF24_GPIO_MISO);	// float

	if (nrf24_check_device())
	{
		while(1)
		{
			NRF24_PORT ^= (1 << NRF24_GPIO_CSN);
			_delay_ms(50);
		}		
	}
	nrf24_flush_tx();
	nrf24_flush_rx();
	nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);			// clear IRQ flags
	nrf24_write_register(NRF24_REG_RF_CH, 100,0x7f);			// channel
	nrf24_write_register(NRF24_REG_SETUP_RETR, 0xff, 0xff);	// delay and retry
	nrf24_write_register(NRF24_REG_CONFIG,0x1,0x1);			// RX
	nrf24_write_register(NRF24_REG_CONFIG,0x2,0x2);			// Power up
	nrf24_write_register(NRF24_REG_EN_AA, 0x1,0x1);			// enable auto ack on pipe0
	nrf24_write_register(NRF24_REG_RX_PW_P0,4,0x3f);			// set payload length of 4 for pipe0
	NRF24_PORT |= (1 << NRF24_GPIO_CE);						// enable device (receiver mode)
	_delay_ms(2);

}

static int nrf24_check_device()
{
	int ret = 0;
	ret = nrf24_get_register(NRF24_REG_STATUS);
	return (ret != NRF24_REG_STATUS_DEFAULT);
}

void nrf24_receive_poll()
{	
	if (!(nrf24_get_register(NRF24_REG_FIFO_STATUS) & 1)) // RX FIFO not empty
	{
		nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);			// clear IRQ flags
		nrf24_read_payload();
	}
}

static void nrf24_read_payload()
{
	int i;
	int ret;
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	_delay_ms(NRF24_SPI_HALF_CLK);
	nrf24_send_byte(NRF24_CMD_R_RX_PAYLOAD);
	for(i=0; i<4; i++)
	{
		ret = nrf24_send_byte(NRF24_CMD_NOP);
		nrf24_dev.payload_buffer[i] = ret;
	}
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}

static void nrf24_transmit_packet(char* payload, uint8_t* status, int* wait)
{
	 
	*status = nrf24_get_register(NRF24_REG_STATUS);	 
	NRF24_PORT &= ~(1 << NRF24_GPIO_CE);
	nrf24_write_payload(payload);	 
	NRF24_PORT |= (1 << NRF24_GPIO_CE);
	do {
		*status = nrf24_get_register(NRF24_REG_STATUS);		 
		if (*status & 0x30)
		break;
	} while ((*wait)--);

	NRF24_PORT &= ~(1 << NRF24_GPIO_CE);
	 
	*status = nrf24_get_register(NRF24_REG_STATUS);
	nrf24_flush_tx();
	*status = nrf24_get_register(NRF24_REG_STATUS);
	nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);
}

static void nrf24_flush_tx(void)
{
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	_delay_ms(NRF24_SPI_HALF_CLK);	 
	nrf24_send_byte(NRF24_CMD_FLUSH_TX);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);	 
}

static void nrf24_flush_rx(void)
{
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	_delay_ms(NRF24_SPI_HALF_CLK);
	nrf24_send_byte(NRF24_CMD_FLUSH_RX);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}
 
static int nrf24_get_register(uint8_t reg)
{
	int ret = 0;
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	_delay_ms(NRF24_SPI_HALF_CLK);
	nrf24_send_byte(NRF24_CMD_R_REGISTER | reg);
	ret = nrf24_send_byte(NRF24_CMD_NOP);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);
	return ret;

}

static void nrf24_get_address_register(uint8_t reg, uint8_t* result)
{
	int i;
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	_delay_ms(NRF24_SPI_HALF_CLK);
	nrf24_send_byte(NRF24_CMD_R_REGISTER | reg);
	for(i=0;i<5;i++)
	{
		*(result + i) = nrf24_send_byte(NRF24_CMD_NOP);
	}
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}

static void nrf24_write_register(uint8_t reg, uint8_t value, uint8_t mask)
{
	int ret;	 
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	_delay_ms(NRF24_SPI_HALF_CLK);
	nrf24_send_byte(NRF24_CMD_R_REGISTER & reg);
	ret = nrf24_send_byte(NRF24_CMD_NOP);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);		 
	_delay_ms(NRF24_SPI_HALF_CLK);
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	_delay_ms(NRF24_SPI_HALF_CLK);
	nrf24_send_byte(NRF24_CMD_W_REGISTER | reg);
	nrf24_send_byte((ret & ~mask) | value);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);	
}

static void nrf24_write_payload(char* payload)
{
	int i;	 
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	_delay_ms(NRF24_SPI_HALF_CLK);
	nrf24_send_byte(NRF24_CMD_W_TX_PAYLOAD);
	for(i=0; i<4; i++)
	{
		nrf24_send_byte(*(payload+i));		 
	}
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);	
}

static int nrf24_send_byte(uint8_t value)
{
	int i=0;
	int ret=0;

	for(i=7;i>=0;i--)
	{
		if (value & (1 << i))
		{			 
			NRF24_PORT |= (1 << NRF24_GPIO_MOSI);	

		} else
		{
			NRF24_PORT &= ~(1 << NRF24_GPIO_MOSI);
		}

		NRF24_PORT &= ~(1 << NRF24_GPIO_SCLK);
		_delay_ms(NRF24_SPI_HALF_CLK);
		if (NRF24_PIN & (1 << NRF24_GPIO_MISO))
		{
		ret = ret | (1 << i );
		}		 		 
		NRF24_PORT |= (1 << NRF24_GPIO_SCLK);	
		_delay_ms(NRF24_SPI_HALF_CLK);
	}

	NRF24_PORT &= ~(1 << NRF24_GPIO_SCLK);
	return ret;
}