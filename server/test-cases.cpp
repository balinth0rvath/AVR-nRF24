#include "test-cases.h"
#include <vector>

void TestLongLoop::run(std::unique_ptr<IWirelessDevice>& device)
{
  std::cout << "TestLongLoop starts to run\n";
  std::vector<char> data{1,2,3,4,0};
  std::vector<char> received(device->getBufferSize(),0);
  for(int i=0;i<1000;i++)
  {
    device->setTransmitter();
    device->send(data);
    device->setReceiver();
    received=device->receive();
    std::cout << "sent: ";
    for(const char& byte : data)
    {
      std::cout << byte << " ";
    }
    std::cout << std::endl;

    std::cout << "received: ";
    for(const char& byte : received)
    {
      std::cout << byte << " ";
    }
    std::cout << std::endl;
  }
  error = true;
}
