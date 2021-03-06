/*
* nrf24.c
*
* Created: 4/30/2021 4:40:35 P
*
* author: balinth0rvath 
*/ 

#include "common.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "nrf24.h"
 
static void nrf24_debug_status(void);
static int nrf24_get_register(uint8_t reg);
static void nrf24_get_address_register(uint8_t reg, uint8_t* result);
static void nrf24_read_payload(void);
static void nrf24_write_register(uint8_t reg, uint8_t value, uint8_t mask);
static void nrf24_write_payload(char* payload);
static void nrf24_write_address(uint8_t reg, uint8_t* address);
static int nrf24_send_byte(uint8_t value);
static int nrf24_bitbang(uint8_t value);
static int nrf24_spi(uint8_t value);

static int g_use_spi = 0;
static uint8_t g_message_received = 0;

static volatile nrf24_payload_buffer_item_t g_payload_buffer_list[NRF24_PAYLOAD_BUFFER_SIZE];
static uint8_t g_payload_buffer_head = 0;
static uint8_t g_payload_buffer_tail = 0;

void nrf24_init(uint8_t use_spi)
{	
  g_use_spi = use_spi;	
  NRF24_DDR |= (1 << NRF24_GPIO_CE);                      // CE output	
  NRF24_DDR |= (1 << NRF24_GPIO_CSN);                     // CSN output
  NRF24_DDR_IRQ &= ~(1 << NRF24_GPIO_IRQ);                // IRQ input

  if (g_use_spi)
  {
    SPCR = 0;
    SPCR |= (1 << SPE);
    SPCR |= (1 << MSTR);
    SPCR &= ~(1 << CPOL);
    SPCR &= ~(1 << CPHA);
    //SPSR |= ~(1 << SPR1);                               // SPI2X SPR1 SPR0 000 20MHz/4 = 5MHz

    DDRB |= (1 << PB2);                                   // SS output
    DDRB |= (1 << PB3);                                   // MOSI PB3 output
    DDRB &= ~(1 << PB4);                                  // MISO PB4 input
    PORTB |= (1 << PB4);                                  // MISO PB4 pull up
    DDRB |= (1 << PB5);                                   // SCLK PB5 output
  } else
  {
    NRF24_DDR |= (1 << NRF24_GPIO_SCLK);                  // SCLK output
    NRF24_DDR |= (1 << NRF24_GPIO_MOSI);                  // MISO output
    NRF24_DDR &= ~(1 << NRF24_GPIO_MISO);                 // MISO input
  }

  NRF24_PORT &= ~(1 << NRF24_GPIO_CE);                    // CE 0 
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);                    // CSN 1

  if (!g_use_spi)
  {
    NRF24_PORT &= ~(1 << NRF24_GPIO_SCLK);                // SCLK 0	
    NRF24_PORT |= (1 << NRF24_GPIO_MISO);                 // MISO pull up	
  }	
			
  NRF24_PORT_IRQ |= (1 << NRF24_GPIO_IRQ);                // IRQ pull up
  MCUCR &= ~(1 << ISC00 | 1 << ISC01);                    // active low IRQ	
	
	
}

void nrf24_set_receiver()
{
	nrf24_flush_tx();
	nrf24_flush_rx();
	NRF24_PORT &= ~(0 << NRF24_GPIO_CE);                    // disable device
	_delay_ms(2);
	nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);       // clear IRQ flags
	nrf24_write_register(NRF24_REG_RF_CH, 100,0x7f);        // channel
	nrf24_write_register(NRF24_REG_SETUP_RETR, 0xff, 0xff); // delay and retry

	nrf24_write_register(NRF24_REG_EN_AA, 0x1,0x1);         // enable auto ack on pipe0
	nrf24_write_register(NRF24_REG_RX_PW_P0,
	NRF24_PAYLOAD_LENGTH, 0x3f);                            // set payload length for pipe0
	nrf24_write_register(NRF24_REG_CONFIG,0x3,0x7);         // CRC 1 byte, RX pwr up
	NRF24_PORT |= (1 << NRF24_GPIO_CE);                     // enable device (receiver mode)
	_delay_ms(2);
}

void nrf24_set_transmitter()
{
	nrf24_flush_tx();
	nrf24_flush_rx();
	NRF24_PORT &= ~(0 << NRF24_GPIO_CE);                    // disable device
	_delay_ms(2);

	nrf24_write_register(NRF24_REG_RF_CH, 100,0x7f);        // channel
	nrf24_write_register(NRF24_REG_SETUP_RETR, 0xff, 0xff); // delay and retry
	nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);       // clear IRQ flags
	nrf24_write_register(NRF24_REG_CONFIG,0x2,0x7);         // CRC 1 byte, TX pwr up	
	_delay_ms(2);
}

int nrf24_check_device()
{		
	return nrf24_get_register(NRF24_REG_STATUS) == 0xff;	
}

void nrf24_power_down()
{
  nrf24_write_register(NRF24_REG_CONFIG,0x0,0x2); 
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

void nrf24_get_buffer_head(nrf24_payload_buffer_item_t* item)
{
	*item = g_payload_buffer_list[g_payload_buffer_head - 1];
}

void nrf24_receive_poll(void)
{	
	if (!(nrf24_get_register(NRF24_REG_FIFO_STATUS) & 1))   // RX FIFO not empty
	{
		nrf24_receive_irq();
	}
}

void nrf24_receive_irq(void)
{	
	nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);       // clear IRQ flags
	nrf24_read_payload();	
	g_message_received = 1;	
}

uint8_t nrf4_message_received(void)
{
	uint8_t ret = g_message_received;
	g_message_received = 0;
	return ret;
}

void nrf24_transmit_irq(void)
{
	volatile int status = 0;
	status = nrf24_get_register(NRF24_REG_STATUS);
	NRF24_PORT &= ~(1 << NRF24_GPIO_CE);
	nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);
}


static void nrf24_read_payload(void)
{
	char payload_buffer[NRF24_PAYLOAD_LENGTH];
	int i;
	int ret;
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	nrf24_send_byte(NRF24_CMD_R_RX_PAYLOAD);
	for(i=0; i < NRF24_PAYLOAD_LENGTH; i++)
	{
		ret = nrf24_send_byte(NRF24_CMD_NOP);
		payload_buffer[i] = ret;
	}
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);
	g_payload_buffer_list[g_payload_buffer_head].id = payload_buffer[0];
	g_payload_buffer_list[g_payload_buffer_head].source_address = payload_buffer[1];
	g_payload_buffer_list[g_payload_buffer_head].value = payload_buffer[2];
	g_payload_buffer_list[g_payload_buffer_head].aux = payload_buffer[3];
	g_payload_buffer_head++;
	if (g_payload_buffer_head == NRF24_PAYLOAD_BUFFER_SIZE)
	{
		g_payload_buffer_head = 0;
	}		
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
	nrf24_send_byte(NRF24_CMD_FLUSH_TX);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);	 
}

void nrf24_flush_rx(void)
{
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	nrf24_send_byte(NRF24_CMD_FLUSH_RX);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}
 
uint8_t nrf24_get_status(void)
{
	int ret = 0;
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	ret = nrf24_send_byte(NRF24_CMD_NOP);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);
	return ret;
}

static int nrf24_get_register(uint8_t reg)
{
	int ret = 0;
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	nrf24_send_byte(NRF24_CMD_R_REGISTER | reg);
	ret = nrf24_send_byte(NRF24_CMD_NOP);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);
	return ret;

}

void nrf24_get_rx_address_p0(uint8_t* address)
{
	nrf24_get_address_register(NRF24_REG_RX_ADDR_P0, address);
}

void nrf24_get_tx_address(uint8_t* address)
{
	nrf24_get_address_register(NRF24_REG_TX_ADDR, address);
}

static void nrf24_get_address_register(uint8_t reg, uint8_t* result)
{
	int i;
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
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
	nrf24_send_byte(NRF24_CMD_R_REGISTER | reg);
	ret = nrf24_send_byte(NRF24_CMD_NOP);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);		 
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	nrf24_send_byte(NRF24_CMD_W_REGISTER | reg);
	nrf24_send_byte((ret & ~mask) | value);
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);	
}

static void nrf24_write_payload(char* payload)
{
	int i;	 	
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	nrf24_send_byte(NRF24_CMD_W_TX_PAYLOAD);	
	for(i=0; i<4; i++)
	{
		nrf24_send_byte(*(payload+i));				 
	}
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);	
}

void nrf24_set_rx_address_p0(uint8_t* address)
{
	nrf24_write_address(NRF24_REG_RX_ADDR_P0, address);
}

void nrf24_set_tx_address(uint8_t* address)
{
	nrf24_write_address(NRF24_REG_TX_ADDR, address);
}


static void nrf24_write_address(uint8_t reg, uint8_t* address)
{
	int i;
	NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	nrf24_send_byte(NRF24_CMD_W_REGISTER | reg);
	for(i=0; i<5; i++)
	{
		nrf24_send_byte(*(address+i));
	}
	NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}

static int nrf24_send_byte(uint8_t value)
{	
	if (g_use_spi)
	{
		return nrf24_spi(value);
	} else
	{
		return nrf24_bitbang(value);
	}
}

static int nrf24_bitbang(uint8_t value)
{
	_delay_ms(NRF24_SPI_HALF_CLK);
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

static int nrf24_spi(uint8_t value)
{
	SPDR = value;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}