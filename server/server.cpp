#include "controller.h"
#include "wireless-device.h"

int main()
{
  Controller controller {};  
  std::unique_ptr<WirelessDevice> device = NRF24DeviceBuilder::create();
  device->setReceiver();
  return 0;
}

