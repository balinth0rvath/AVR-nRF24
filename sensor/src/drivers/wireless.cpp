/*
 * wireless.cpp
 *
 * Created: 10/25/2021 12:15:36 PM
 *  Author: balinth0horvath
 */ 

#include <avr/io.h>
#include "common.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "wireless.h"

// Settings
#define NRF24_SET_RECEIVER                0x1
#define NRF24_PAYLOAD_LENGTH              0x4
#define NRF24_PAYLOAD_BUFFER_SIZE         0x50

// SPI Port
#define NRF24_DDR                         DDRC
#define NRF24_PORT                        PORTC
#define NRF24_PIN                         PINC

// IRQ Port
#define NRF24_DDR_IRQ                     DDRD
#define NRF24_PORT_IRQ                    PORTD

// SPI&CE pin settings
#define NRF24_GPIO_CE                     PC2
#define NRF24_GPIO_SCLK                   PC4
#define NRF24_GPIO_MOSI                   PC5
#define NRF24_GPIO_CSN                    PC1
#define NRF24_GPIO_MISO                   PC3
#define NRF24_SPI_HALF_CLK                1

// IRQ pin settings
#define NRF24_GPIO_IRQ                    PD2

// nRF24 commands
#define NRF24_CMD_R_REGISTER              0x00
#define NRF24_CMD_W_REGISTER              0x20
#define NRF24_CMD_R_RX_PAYLOAD            0x61
#define NRF24_CMD_W_TX_PAYLOAD            0xa0
#define NRF24_CMD_FLUSH_TX                0xe1
#define NRF24_CMD_FLUSH_RX                0xe2
#define NRF24_CMD_REUSE_TX_PL             0xe3
#define NRF24_CMD_ACTIVATE                0x50
#define NRF24_CMD_R_RX_PL_WID             0x60
#define NRF24_CMD_W_ACK_PAYLOAD           0xa8
#define NRF24_CMD_W_TX_PAYLOAD_NO_ACK     0xb0
#define NRF24_CMD_NOP                     0xff

// nRF24 registers
#define NRF24_REG_CONFIG                  0x00
#define NRF24_REG_EN_AA                   0x01
#define NRF24_REG_EN_RXADDR               0x02
#define NRF24_REG_SETUP_AW                0x03
#define NRF24_REG_SETUP_RETR              0x04
#define NRF24_REG_RF_CH                   0x05
#define NRF24_REG_RF_SETUP                0x06
#define NRF24_REG_STATUS                  0x07
#define NRF24_REG_OBSERVE_TX              0x08
#define NRF24_REG_CD                      0x09
#define NRF24_REG_RX_ADDR_P0              0x0a
#define NRF24_REG_RX_ADDR_P1              0x0b
#define NRF24_REG_RX_ADDR_P2              0x0c
#define NRF24_REG_RX_ADDR_P3              0x0d
#define NRF24_REG_RX_ADDR_P4              0x0e
#define NRF24_REG_RX_ADDR_P5              0x0f
#define NRF24_REG_TX_ADDR                 0x10
#define NRF24_REG_RX_PW_P0                0x11
#define NRF24_REG_RX_PW_P1                0x12
#define NRF24_REG_RX_PW_P2                0x13
#define NRF24_REG_RX_PW_P3                0x14
#define NRF24_REG_RX_PW_P4                0x15
#define NRF24_REG_RX_PW_P5                0x16
#define NRF24_REG_FIFO_STATUS             0x17
#define NRF24_REG_DYNPD                   0x1c
#define NRF24_REG_FEATURE                 0x1d

#define NRF24_REG_STATUS_DEFAULT          0x0e

WirelessDevice* pWirelessDevice;

WirelessDevice::WirelessDevice()
{
  pWirelessDevice = this;  
}

void WirelessDevice::init(bool useSpi)
{
  this->useSpi = useSpi;
  NRF24_DDR |= (1 << NRF24_GPIO_CE);                      // CE output
  NRF24_DDR |= (1 << NRF24_GPIO_CSN);                     // CSN output
  NRF24_DDR_IRQ &= ~(1 << NRF24_GPIO_IRQ);                // IRQ input

  if (useSpi)
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

  if (!useSpi)
  {
    NRF24_PORT &= ~(1 << NRF24_GPIO_SCLK);                // SCLK 0
    NRF24_PORT |= (1 << NRF24_GPIO_MISO);                 // MISO pull up
  }
  
  NRF24_PORT_IRQ |= (1 << NRF24_GPIO_IRQ);                // IRQ pull up
  MCUCR &= ~(1 << ISC00 | 1 << ISC01);                    // active low IRQ
  
}

void WirelessDevice::getRXAddressP0(unsigned char* address)
{
  getAddressRegister(NRF24_REG_RX_ADDR_P0, address);
}

void WirelessDevice::getTXAddress(unsigned char* address)
{
  getAddressRegister(NRF24_REG_TX_ADDR, address);
}


void WirelessDevice::setTXAddress(unsigned char* address)
{
	writeAddress(NRF24_REG_TX_ADDR, address);
}


void WirelessDevice::setRxAddressP0(unsigned char* address)
{
  writeAddress(NRF24_REG_RX_ADDR_P0, address);
}

void WirelessDevice::setReceiver()
{
  flushTX();
  flushRX();
  NRF24_PORT &= ~(0 << NRF24_GPIO_CE);                    // disable device
  _delay_ms(2);
  writeRegister(NRF24_REG_STATUS,0x70,0x70);       // clear IRQ flags
  writeRegister(NRF24_REG_RF_CH, 100,0x7f);        // channel
  writeRegister(NRF24_REG_SETUP_RETR, 0xff, 0xff); // delay and retry

  writeRegister(NRF24_REG_EN_AA, 0x1,0x1);         // enable auto ack on pipe0
  writeRegister(NRF24_REG_RX_PW_P0,
  NRF24_PAYLOAD_LENGTH, 0x3f);                            // set payload length for pipe0
  writeRegister(NRF24_REG_CONFIG,0x3,0x7);         // CRC 1 byte, RX pwr up
  NRF24_PORT |= (1 << NRF24_GPIO_CE);                     // enable device (receiver mode)
  _delay_ms(2);
}

void WirelessDevice::setTransmitter()
{
  flushTX();
  flushRX();
  NRF24_PORT &= ~(0 << NRF24_GPIO_CE);                    // disable device
  _delay_ms(2);

  writeRegister(NRF24_REG_RF_CH, 100,0x7f);        // channel
  writeRegister(NRF24_REG_SETUP_RETR, 0xff, 0xff); // delay and retry
  writeRegister(NRF24_REG_STATUS,0x70,0x70);       // clear IRQ flags
  writeRegister(NRF24_REG_CONFIG,0x2,0x7);         // CRC 1 byte, TX pwr up
  _delay_ms(2);
}

bool WirelessDevice::checkDevice()
{
  return getRegister(NRF24_REG_STATUS) != 0xff;
}

void WirelessDevice::powerDown()
{
  writeRegister(NRF24_REG_CONFIG,0x0,0x2);
}

void WirelessDevice::getBufferHead(PayloadBufferItem* item)
{
  *item = payloadBufferList[payloadBufferHead - 1];
}

void WirelessDevice::receivePoll()
{
  if (!(getRegister(NRF24_REG_FIFO_STATUS) & 1))   // RX FIFO not empty
  {
    receiveIRQ();
  }
}

void WirelessDevice::receiveIRQ()
{
  writeRegister(NRF24_REG_STATUS,0x70,0x70);              // clear IRQ flags
  readPayload();
  messageReceived = true;
}

bool WirelessDevice::isMessageReceived()
{
  bool ret = messageReceived;
  messageReceived = 0;
  return ret;
}

void WirelessDevice::transmitIRQ()
{
  volatile int status = 0;
  status = getRegister(NRF24_REG_STATUS);
  NRF24_PORT &= ~(1 << NRF24_GPIO_CE);
  writeRegister(NRF24_REG_STATUS,0x70,0x70);
}

void WirelessDevice::readPayload()
{
  char payloadBuffer[NRF24_PAYLOAD_LENGTH];
  int i;
  int ret;
  NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
  sendByte(NRF24_CMD_R_RX_PAYLOAD);
  for(i=0; i < NRF24_PAYLOAD_LENGTH; i++)
  {
    ret = sendByte(NRF24_CMD_NOP);
    payloadBuffer[i] = ret;
  }
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);
  payloadBufferList[payloadBufferHead].id = payloadBuffer[0];
  payloadBufferList[payloadBufferHead].sourceAddress = payloadBuffer[1];
  payloadBufferList[payloadBufferHead].value = payloadBuffer[2];
  payloadBufferList[payloadBufferHead].aux = payloadBuffer[3];
  payloadBufferHead++;
  if (payloadBufferHead == NRF24_PAYLOAD_BUFFER_SIZE)
  {
    payloadBufferHead = 0;
  }
}

void WirelessDevice::transmitPacket(char* payload, unsigned char* status)
{
  *status = getRegister(NRF24_REG_STATUS);
  NRF24_PORT &= ~(1 << NRF24_GPIO_CE);
  writePayload(payload);
  NRF24_PORT |= (1 << NRF24_GPIO_CE);
}

void WirelessDevice::flushTX()
{
  NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
  sendByte(NRF24_CMD_FLUSH_TX);
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}

void WirelessDevice::flushRX()
{
  NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
  sendByte(NRF24_CMD_FLUSH_RX);
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}

unsigned char WirelessDevice::getStatus()
{
  int ret = 0;
  NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
  ret = sendByte(NRF24_CMD_NOP);
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);
  return ret;
}

void WirelessDevice::debugStatus()
{
  volatile int a,i,reg;
  for (i=0;i<0x18;i++)
  {
    reg = getRegister(i);
    reg = reg + a;
  }
}

int WirelessDevice::getRegister(unsigned char reg)
{
  int ret = 0;
  NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
  sendByte(NRF24_CMD_R_REGISTER | reg);
  ret = sendByte(NRF24_CMD_NOP);
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);
  return ret;
}

void WirelessDevice::getAddressRegister(unsigned char reg, unsigned char* result)
{
  int i;
  NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
  sendByte(NRF24_CMD_R_REGISTER | reg);
  for(i=0;i<5;i++)
  {
    *(result + i) = sendByte(NRF24_CMD_NOP);
  }
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}

void WirelessDevice::writeRegister(unsigned char reg, unsigned char value, unsigned char mask)
{
  int ret;
  NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
  sendByte(NRF24_CMD_R_REGISTER | reg);
  ret = sendByte(NRF24_CMD_NOP);
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);
  NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
  sendByte(NRF24_CMD_W_REGISTER | reg);
  sendByte((ret & ~mask) | value);
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}

void WirelessDevice::writePayload(char* payload)
{
  int i;
  NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
  sendByte(NRF24_CMD_W_TX_PAYLOAD);
  for(i=0; i<4; i++)
  {
    sendByte(*(payload+i));
  }
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}

void WirelessDevice::writeAddress(unsigned char reg, unsigned char* address)
{
  int i;
  NRF24_PORT &= ~(1 << NRF24_GPIO_CSN);
  sendByte(NRF24_CMD_W_REGISTER | reg);
  for(i=0; i<5; i++)
  {
    sendByte(*(address+i));
  }
  NRF24_PORT |= (1 << NRF24_GPIO_CSN);
}


int WirelessDevice::sendByte(unsigned char value)
{
  if(this->useSpi)
  {
    return sendByteSPI(value);
  } else
  {
    return sendByteBitBang(value);
  }
}

int WirelessDevice::sendByteBitBang(unsigned char value)
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

int WirelessDevice::sendByteSPI(unsigned char value)
{
  SPDR = value;
  while (!(SPSR & (1 << SPIF)));
  return SPDR;
}

ISR (INT0_vect)
{
  volatile unsigned char ret = pWirelessDevice->getStatus();
  if (ret & 0x40)
  {
    pWirelessDevice->receiveIRQ();
  } else if (ret & 0x20)
  {
    pWirelessDevice->transmitIRQ();
  }
}

