/*
* nrf24.c
*
* Created: 4/30/2021 4:40:35 PM
*/ 
#include "common.h"
#include <avr/io.h>
#include <util/delay.h>			// add this to use the delay function
#include "nrf24.h"
 
static int 	nrf24_check_device();
static void nrf24_debug_status(void);
static int nrf24_get_register(uint8_t reg);
static void nrf24_get_address_register(uint8_t reg, uint8_t* result);
static void nrf24_read_payload(void);
static void nrf24_write_register(uint8_t reg, uint8_t value, uint8_t mask);
static void nrf24_write_payload(char* payload);
static int 	nrf24_send_byte(uint8_t value);
static int nrf24_bitbang(uint8_t value);

struct nrf24_dev_t {
	char payload_buffer[32];
} nrf24_dev;

static int g_use_spi = 0;

void nrf24_init(uint8_t set_receiver, uint8_t use_spi)
{	
	g_use_spi = use_spi;	
	NRF24_DDR |= (1 << NRF24_GPIO_CE);							// CE output	
	NRF24_DDR |= (1 << NRF24_GPIO_CSN);							// CSN output
	NRF24_DDR_IRQ &= ~(1 << NRF24_GPIO_IRQ);					// IRQ input

	if (g_use_spi)
	{

	} else
	{
		NRF24_DDR |= (1 << NRF24_GPIO_SCLK);						// SCLK output
		NRF24_DDR |= (1 << NRF24_GPIO_MOSI);						// MISO output
		NRF24_DDR &= ~(1 << NRF24_GPIO_MISO);						// MISO input
	}

	NRF24_PORT &= ~(1 << NRF24_GPIO_CE);						// CE 0 
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);						// CSN 1

	if (!g_use_spi)
	{
		NRF24_PORT &= ~(1 << NRF24_GPIO_SCLK);						// SCLK 0	
	}	

	NRF24_PORT != (1 << NRF24_GPIO_MISO);						// MISO pull up			
	NRF24_PORT_IRQ |= (1 << NRF24_GPIO_IRQ);					// IRQ pull up

	MCUCR &= ~(1 << ISC00 | 1 << ISC01);						// active low IRQ	

	nrf24_flush_tx();
	nrf24_flush_rx();
	
	nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);			// clear IRQ flags
	nrf24_write_register(NRF24_REG_RF_CH, 100,0x7f);			// channel	
	nrf24_write_register(NRF24_REG_SETUP_RETR, 0xff, 0xff);		// delay and retry

	if (set_receiver)
	{	
		nrf24_write_register(NRF24_REG_EN_AA, 0x1,0x1);			// enable auto ack on pipe0
		nrf24_write_register(NRF24_REG_RX_PW_P0,4,0x3f);		// set payload length of 4 for pipe0	    
		nrf24_write_register(NRF24_REG_CONFIG,0x3,0x7);			// CRC 1 byte, RX pwr up			
		NRF24_PORT |= (1 << NRF24_GPIO_CE);						// enable device (receiver mode)
	} else
	{	
		nrf24_write_register(NRF24_REG_CONFIG,0x2,0x7);			// CRC 1 byte, TX pwr up	
	}			
	_delay_ms(2);

}

static int nrf24_check_device()
{
	int ret = 0;
	ret = nrf24_get_register(NRF24_REG_STATUS);
	return (ret != NRF24_REG_STATUS_DEFAULT);	
}

static void nrf24_debug_status(void)
{
	volatile int a,i,reg;
	for (i=0;i<0x18;i++)
	{
		reg = nrf24_get_register(i);
		reg = reg + a;
	}
}

void nrf24_receive_poll(void)
{	
	if (!(nrf24_get_register(NRF24_REG_FIFO_STATUS) & 1)) // RX FIFO not empty
	{
		nrf24_receive_irq();
	}
}

void nrf24_receive_irq(void)
{	
	nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);			// clear IRQ flags
	nrf24_read_payload();	
}

void nrf24_transmit_irq(void)
{
	int status = 0;
	status = nrf24_get_register(NRF24_REG_STATUS);
	NRF24_PORT &= ~(1 << NRF24_GPIO_CE);
	
	nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);
}


static void nrf24_read_payload(void)
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

void nrf24_transmit_packet(char* payload, uint8_t* status)
{ 
	*status = nrf24_get_register(NRF24_REG_STATUS);	 
	NRF24_PORT &= ~(1 << NRF24_GPIO_CE);
	nrf24_write_payload(payload);	 
	NRF24_PORT |= (1 << NRF24_GPIO_CE);		
}

void nrf24_flush_tx(void)
{
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	_delay_ms(NRF24_SPI_HALF_CLK);	 
	nrf24_send_byte(NRF24_CMD_FLUSH_TX);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);	 
}

void nrf24_flush_rx(void)
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
	nrf24_send_byte(NRF24_CMD_R_REGISTER | reg);
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
	if (g_use_spi)
	{

	} else
	{
		return nrf24_bitbang(value);
	}
}

static int nrf24_bitbang(uint8_t value)
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