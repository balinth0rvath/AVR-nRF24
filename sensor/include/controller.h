/*
 * controller.h
 *
 * Created: 10/23/2021 3:57:26 PM
 *  Author: balinth0horvath
 */ 


#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "wireless.h"

class ControlLight {
  public:
  ControlLight();
  void blink(unsigned char number);
};

class Controller {
public:
  Controller();
  void runLongLoop();
private:
  void sendResponse(char* payload);
  void sendResponseWithHead();
  void initIRQ();
  WirelessDevice wirelessDevice;
  ControlLight controlLight;

};




#endif /* CONTROLLER_H_ */