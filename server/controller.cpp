#include "controller.h"

Controller::Controller() {
  std::vector<char> address = {0x10,0x20,0x30,0x40,0x50};
  device = NRF24DeviceBuilder::create(address);
  device->setTransmitter();
}
