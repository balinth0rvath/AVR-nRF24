#include <memory>
#include "wireless-device.h"

class Controller {
public:
  Controller();
private:
  std::unique_ptr<WirelessDevice> device;
};

