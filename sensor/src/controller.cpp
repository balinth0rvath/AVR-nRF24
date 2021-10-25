/*
 * controller.cpp
 *
 * Created: 10/23/2021 3:57:13 PM
 *  Author: balinth0horvath
 */ 

 #include "controller.h"
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "common.h"
 #include <util/delay.h>
 #include <avr/io.h>

 Controller::Controller()
 {
  unsigned char address[5]={0x10,0x20,0x30,0x40,0x50};
  wirelessDevice.init(false);
  wirelessDevice.setTXAddress(address);
  wirelessDevice.setRxAddressP0(address);

  if(!wirelessDevice.checkDevice())
  {
    controlLight.blink(100);
  }
  wirelessDevice.setReceiver();

  initIRQ();       
 }

 void Controller::runLongLoop()
 {
   while(1)
   {
     _delay_ms(10);
     if (wirelessDevice.isMessageReceived())
     {
       wirelessDevice.setTransmitter();
       sendResponseWithHead();      
       controlLight.blink(8);  
       wirelessDevice.setReceiver();
     }
   }
 }

 void Controller::sendResponse(char* payload)
 {
   unsigned char status;
   wirelessDevice.transmitPacket(payload, &status);
   _delay_ms(100);   
   wirelessDevice.transmitPacket(payload, &status);
   _delay_ms(100);   
 }

 void Controller::sendResponseWithHead()
 {
   char payload[4] = {};
   PayloadBufferItem p;
   wirelessDevice.getBufferHead(&p);
   payload[0] = p.id;
   payload[1] = p.sourceAddress;
   payload[2] = p.value;
   payload[3] = p.aux;
   sendResponse(payload);
 }

void Controller::initIRQ()
{
  EICRA &= ~(1 << ISC00 | 1 << ISC01);
  EIMSK |= (1 << INT0);
  sei();
}

 ControlLight::ControlLight()
 {
  DDRD|=( 1 << PD0); 
  PORTD &= ~(1 << PD0);
  PORTD |= 1 << PD0;
  _delay_ms(500);
  PORTD &= ~(1 << PD0);
 }

 void ControlLight::blink(unsigned char number)
 {
 int i=0;
	for(i=0;i<number;i++)
	{
		PORTD ^= (1 << PD0);
		_delay_ms(50);		  
  }  
 }