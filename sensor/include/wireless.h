/*
 * wireless.h
 *
 * Created: 10/25/2021 12:15:21 PM
 *  Author: balinth0horvath
 */ 

#ifndef WIRELESS_H_
#define WIRELESS_H_

#define NRF24_PAYLOAD_BUFFER_SIZE         0x50

struct PayloadBufferItem {
  unsigned char id;
  unsigned char sourceAddress;
  unsigned char value;
  unsigned char aux;
};

class WirelessDevice {
public:
  WirelessDevice();
  void init(bool useSpi);
  void getRXAddressP0(unsigned char* address);
  void getTXAddress(unsigned char* address);
  void setTXAddress(unsigned char* address);
  void setRxAddressP0(unsigned char* address);
  void setReceiver();
  void setTransmitter();
  bool checkDevice();  
  void powerDown();    
  void getBufferHead(PayloadBufferItem* item);
  void receivePoll();
  void receiveIRQ();
  bool isMessageReceived();
  void transmitIRQ();
  void readPayload();
  void transmitPacket(char* payload, unsigned char* status);
  void flushTX();
  void flushRX();  
  unsigned char getStatus();
  void debugStatus();
private:  
  int getRegister(unsigned char reg);
  void getAddressRegister(unsigned char reg, unsigned char* result);
  void writeRegister(unsigned char reg, unsigned char value, unsigned char mask);
  void writePayload(char* payload);
  void writeAddress(unsigned char reg, unsigned char* address);
  int sendByte(unsigned char value);
  int sendByteBitBang(unsigned char value);
  int sendByteSPI(unsigned char value);

  PayloadBufferItem payloadBufferList[NRF24_PAYLOAD_BUFFER_SIZE];
  int payloadBufferHead;
  int payloadBufferTail;
  bool useSpi;
  bool messageReceived;

};

#endif /* WIRELESS_H_ */