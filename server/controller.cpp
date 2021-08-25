#include <iostream>
#include <thread>
#include <chrono>
#include "controller.h"

Controller::Controller() {
  using namespace std::chrono_literals;
  std::vector<char> address = {0x10,0x20,0x30,0x40,0x50};
  device = NRF24DeviceBuilder::create(address);
  /*
  device->setTransmitter();
  std::vector<char> data = {1,2,3,4,0};
  device->send(data);
  */
  device->setReceiver();
  while(true)
  {
    std::vector<char> data = device->receive(); 
    std::cout << "Received: ";
    for(const char& byte : data)
    {
      std::cout << byte << " ";
    } 
    std::cout << std::endl;
    std::this_thread::sleep_for(500ms);
  } 
}
