#include <iostream>
#include <thread>
#include <chrono>
#include "controller.h"
#include "test-cases.h"

Controller::Controller() {
  std::cout << "Controller::Controller\n";
  /*
  device->setTransmitter();
  std::vector<char> data = {1,2,3,4,0};
  device->send(data);
  */
  device->setReceiver();

}

void Controller::run()
{
  using namespace std::chrono;
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

TestController::TestController() {
  std::cout << "TestController::TestController\n";
}

void TestController::run()
{
  TestLongLoop test {};
  device->setTransmitter();
  test.run(device);
  if (test.passed())
  {
    std::cout << "Test longloop passed\n";
  } else
  {
    std::cout << "Test longloop failed\n";
  }
}

ControllerBase::ControllerBase() {
  std::cout << "ControllerBase::ControllerBase\n";
  device = NRF24DeviceBuilder::create(defaultAddress);
  device->setTransmitter();
}
