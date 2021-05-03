/*
 * nrf24.h
 *
 * Created: 4/30/2021 4:40:18 PM
 */ 


#ifndef NRF24_H_
#define NRF24_H_

#include <avr/io.h>

// Settings
#define NRF24_SET_RECEIVER						0x0
#define NRF24_SET_TRANSMITTER					0x1

// nRF24 Port
#define NRF24_DDR								DDRC
#define NRF24_PORT								PORTC
#define NRF24_PIN								PINC

// GPIO pin settings
#define NRF24_GPIO_CE 							PC2							
#define NRF24_GPIO_SCLK							PC4								
#define NRF24_GPIO_MOSI							PC5								
#define NRF24_GPIO_CSN							PC1								
#define NRF24_GPIO_IRQ	 						PC0								
#define NRF24_GPIO_MISO							PC3							
#define NRF24_SPI_HALF_CLK						5								

// nRF24 commands
#define NRF24_CMD_R_REGISTER 					0x00
#define NRF24_CMD_W_REGISTER 					0x20
#define NRF24_CMD_R_RX_PAYLOAD					0x61
#define NRF24_CMD_W_TX_PAYLOAD					0xa0
#define NRF24_CMD_FLUSH_TX 						0xe1
#define NRF24_CMD_FLUSH_RX 						0xe2
#define NRF24_CMD_REUSE_TX_PL 					0xe3
#define NRF24_CMD_ACTIVATE						0x50
#define NRF24_CMD_R_RX_PL_WID 					0x60
#define NRF24_CMD_W_ACK_PAYLOAD 				0xa8
#define NRF24_CMD_W_TX_PAYLOAD_NO_ACK			0xb0
#define NRF24_CMD_NOP 							0xff

// nRF24 registers
#define NRF24_REG_CONFIG						0x00
#define NRF24_REG_EN_AA							0x01
#define NRF24_REG_EN_RXADDR						0x02
#define NRF24_REG_SETUP_AW						0x03
#define NRF24_REG_SETUP_RETR					0x04
#define NRF24_REG_RF_CH							0x05
#define NRF24_REG_RF_SETUP						0x06
#define NRF24_REG_STATUS						0x07
#define NRF24_REG_OBSERVE_TX					0x08
#define NRF24_REG_CD							0x09
#define NRF24_REG_RX_ADDR_P0					0x0a
#define NRF24_REG_RX_ADDR_P1					0x0b
#define NRF24_REG_RX_ADDR_P2					0x0c
#define NRF24_REG_RX_ADDR_P3					0x0d
#define NRF24_REG_RX_ADDR_P4					0x0e
#define NRF24_REG_RX_ADDR_P5					0x0f
#define NRF24_REG_TX_ADDR						0x10
#define NRF24_REG_RX_PW_P0						0x11
#define NRF24_REG_RX_PW_P1						0x12
#define NRF24_REG_RX_PW_P2						0x13
#define NRF24_REG_RX_PW_P3						0x14
#define NRF24_REG_RX_PW_P4						0x15
#define NRF24_REG_RX_PW_P5						0x16
#define NRF24_REG_FIFO_STATUS					0x17
#define NRF24_REG_DYNPD							0x1c
#define NRF24_REG_FEATURE						0x1d

#define NRF24_REG_STATUS_DEFAULT				0x0e

void nrf24_init(uint8_t direction);
void nrf24_receive_poll(void);
static int 	nrf24_check_device(void);
static int nrf24_get_register(uint8_t reg);
static void nrf24_get_address_register(uint8_t reg, uint8_t* result);
static void nrf24_read_payload(void);
static void nrf24_write_register(uint8_t reg, uint8_t value, uint8_t mask);
static void nrf24_write_payload(char* payload);
static void nrf24_flush_tx(void);
static void nrf24_flush_rx(void);
static int 	nrf24_send_byte(uint8_t value);
static void nrf24_transmit_packet(char* payload, uint8_t* status, int* wait);

#endif /* NRF24_H_ */