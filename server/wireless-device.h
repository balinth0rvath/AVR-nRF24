class WirelessDevice {
public:
  virtual void open() = 0;
  virtual void close() = 0;
  virtual void setReceiver() = 0;
  virtual void setTransmitter() = 0;
  virtual void setAddress() = 0;
private:
  int fd = 0;
};
