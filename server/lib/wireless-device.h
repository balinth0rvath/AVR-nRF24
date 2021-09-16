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
  virtual ~WirelessDevice() {};
};

class NRF24Device : public WirelessDevice {
public:
  NRF24Device();
  ~NRF24Device();
  virtual void setReceiver();
  virtual void setTransmitter();
  virtual void setAddress(const std::vector<char>& address);
  virtual int send(const std::vector<char>& data);
  //https://web.archive.org/web/20130930101140/http://cpp-next.com/archive/2009/08/want-speed-pass-by-value
  virtual std::vector<char> receive();
private:
  static constexpr const char * deviceName = "/dev/nrf24d";
  static constexpr const int bufferSize = 64;
  int fd = 0;
};

class NRF24DeviceBuilder {
public:
  static std::unique_ptr<NRF24Device> create();
  static std::unique_ptr<NRF24Device> create(const std::vector<char>& address);
};
