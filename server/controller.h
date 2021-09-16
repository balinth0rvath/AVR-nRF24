#include <memory>
#include "lib/wireless-device.h"

class ControllerBase {
public:
  ControllerBase();
  virtual void run() = 0;
protected:
  std::unique_ptr<IWirelessDevice> device;
private:
  const std::vector<char> defaultAddress = {0x10,0x20,0x30,0x40,0x50};
};

class Controller : public ControllerBase {
public:
  Controller();
  void run();
};

class TestController : public ControllerBase {
public: 
  TestController();
  void run();
};

