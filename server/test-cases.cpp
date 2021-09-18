#include "test-cases.h"
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>

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
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration));
    received=device->receive();
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration));

    if (std::equal(data.begin(), data.end()-1, received.begin()))
    {
      std::cout << "Count: :" << i << ": pass\n";
    } else
    {
      std::cout << " FAIL:\n";
      for(const char& byte : received)
      {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)byte << " ";
      }
      error = true;
    }
  }
}

