#include <vector>
#include <iostream>
#include <memory>

class WirelessDevice {
public:
  virtual void setReceiver() = 0;
  virtual void setTransmitter() = 0;
  virtual void setAddress(const std::vector<char>& address) = 0;
  virtual int send(const std::vector<char>& data) = 0;
  virtual std::vector<char> receive() = 0;
  virtual ~WirelessDevice() { std::cout << "WirelessDevice Base destructor called! \n";};
};

class NRF24Device : public WirelessDevice {
public:
  NRF24Device();
  ~NRF24Device();
  virtual void setReceiver();
  virtual void setTransmitter();
  virtual void setAddress(const std::vector<char>& address);
  virtual int send(const std::vector<char>& data);
  virtual std::vector<char> receive();
private:
  static constexpr const char * deviceName = "/dev/nrf24d";
  int fd = 0;
};

class NRF24DeviceBuilder {
public:
  static std::unique_ptr<NRF24Device> create();
};
