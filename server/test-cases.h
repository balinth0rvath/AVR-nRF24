#include <memory>
#include "lib/wireless-device.h"

class ITestCase {
public:
  virtual void run(std::unique_ptr<WirelessDevice>& device) = 0;
  virtual bool passed() = 0;
  virtual ~ITestCase() {};
};

class TestLongLoop : public ITestCase {
public:
  virtual void run(std::unique_ptr<WirelessDevice>& device);
  virtual bool passed() { return !error; };
private:
  bool error = false;
};
