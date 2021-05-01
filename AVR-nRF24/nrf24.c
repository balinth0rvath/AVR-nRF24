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
 }

 static void nrf24_transmit_packet(char* payload, int* status, int* wait)
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

 static void nrf24_init_device()
 {
	 
	 nrf24_flush_tx();	 
	 nrf24_write_register(NRF24_REG_STATUS,0x70,0x70);
	 nrf24_write_register(NRF24_REG_RF_CH, 100,0x7f);
	 nrf24_write_register(NRF24_REG_SETUP_RETR, 0xff, 0xff);	 
	 nrf24_write_register(NRF24_REG_CONFIG,0x2,0x2);
	 _delay_ms(2);
 }

 static int nrf24_check_device()
 {
	 int ret = 0;
	 ret = nrf24_get_register(NRF24_REG_STATUS);
	 return (ret != NRF24_REG_STATUS_DEFAULT);
 }

 static void nrf24_flush_tx(void)
 {
	 NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	 _delay_ms(NRF24_SPI_HALF_CLK);	 
	 nrf24_send_byte(NRF24_CMD_FLUSH_TX);
	 NRF24_PORT |= (1 << NRF24_GPIO_CSN);	 
 }

 static int nrf24_get_register(int reg)
 {
	 int ret = 0;
	 NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
	 _delay_ms(NRF24_SPI_HALF_CLK);
	 nrf24_send_byte(NRF24_CMD_R_REGISTER | reg);
	 ret = nrf24_send_byte(NRF24_CMD_NOP);
	 NRF24_PORT |= (1 << NRF24_GPIO_CSN);
	 return ret;

 }

 static void nrf24_get_address_register(int reg, int* result)
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

 static void nrf24_write_register(int reg, int value, int mask)
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

 static int nrf24_send_byte(int value)
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