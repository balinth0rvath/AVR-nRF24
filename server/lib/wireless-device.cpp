#include "wireless-device.h"
#include <cstdlib>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <sys/ioctl.h>
#include <exception>

#define NRF24_IOCTL_SET_RECEIVER _IO(153,0)
#define NRF24_IOCTL_SET_TRANSMITTER _IO(153,1)
#define NRF24_IOCTL_SET_ADDRESS _IOW(153,2, struct ADDRESS_T)

struct ADDRESS_T {
  unsigned char octet[5];
};

NRF24Device::NRF24Device()
{
  fd = open(deviceName, O_RDWR);
  if (fd==-1)
  {
    throw std::exception();
  }
  std::cout << "Device opened\n";
}

NRF24Device::~NRF24Device()
{
  
  int ret = close(fd);
  if (ret!=0)
  {
    std::cout << "~NRF24Device close failed\n";
  } 
  std::cout << "Device closed\n";
}

void NRF24Device::setReceiver()
{
  int ret = ioctl(fd, NRF24_IOCTL_SET_RECEIVER, 0);
  if (ret == -1)
  {
    throw std::exception();
  }
}

void NRF24Device::setTransmitter()
{
  int ret = ioctl(fd, NRF24_IOCTL_SET_TRANSMITTER, 0);
  if (ret == -1)
  {
    throw std::exception();
  }
}

void NRF24Device::setAddress(const std::vector<char>& address)
{
  ADDRESS_T addressStruct;
  int i=0;
  for(const char& octet : address)
  {
    addressStruct.octet[i++]=octet;
  }
  int ret = ioctl(fd, NRF24_IOCTL_SET_ADDRESS, (void*)&addressStruct);
  if (ret == -1)
  {
    throw std::exception();
  }
}

int NRF24Device::send(const std::vector<char>& data)
{
  int retval = write(fd,reinterpret_cast<const char*>(&data[0]), data.size());
  if (retval != data.size())
  {
    throw std::exception();
  }
  return retval;

}
std::vector<char> NRF24Device::receive()
{
  std::vector<char> data(bufferSize);
  int count = read(fd, &data[0], bufferSize);
  return data;
};

std::unique_ptr<NRF24Device> NRF24DeviceBuilder::create() 
{
  return std::make_unique<NRF24Device>(); 
}


std::unique_ptr<NRF24Device> NRF24DeviceBuilder::create(const std::vector<char>& address)
{
  std::unique_ptr<NRF24Device> device = std::make_unique<NRF24Device>();
  device->setAddress(address);
  return device;
}
