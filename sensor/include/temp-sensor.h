/*
 * temp_sensor.h
 *
 * Created: 10/26/2021 5:43:42 PM
 *  Author: balinth0horvath
 */ 


#ifndef TEMPSENSOR_H_
#define TEMPSENSOR_H_

#include <avr/io.h>

// Data Port
#define DS18B20_DDR                             DDRB
#define DS18B20_PORT                            PORTB
#define DS18B20_PIN                             PINB

// Pin settings
#define DS18B20_DQ                              PB0

// DS18B20 commands
#define DS18B20_CMD_SEARCH_ROM                  0xf0
#define DS18B20_CMD_READ_ROM                    0x33
#define DS18B20_CMD_MATCH_ROM                   0x55
#define DS18B20_CMD_SKIP_ROM                    0xcc
#define DS18B20_CMD_ALARM_SEARCH                0xec
#define DS18B20_CMD_CONVERT_T                   0x44
#define DS18B20_CMD_READ_SCRATCHPAD             0xbe
#define DS18B20_CMD_WRITE_SCRATCHPAD            0x4e
#define DS18B20_CMD_COPY_SCRATCHPAD             0x48
#define DS18B20_CMD_RECALL_E                    0xb8
#define DS18B20_CMD_READ_POWER_SUPPLY           0xb4


// DS18B20 registers
#define DS18B20_REG_TEMP_LSB                    0x00
#define DS18B20_REG_TEMP_MSB                    0x01
#define DS18B20_REG_TH                          0x02
#define DS18B20_REG_TL                          0x03
#define DS18B20_REG_CONFIG                      0x04
#define DS18B20_REG_CRC                         0x08

// DS18B20 timings
// Init procedure
#define DS18B20_TM_RESET_PULSE_US               500
#define DS18B20_TM_WAIT_RESET_US                15
#define DS18B20_TM_PRESENCE_SLICE_US            20              
#define DS18B20_TM_PRESENCE_COUNT               15
#define DS18B20_TM_WAIT_PRESENCE_US             200

// Time slot timings
#define DS18B20_TM_WRITE_FULL_SLOT_LENGTH_US    80
#define DS18B20_TM_WRITE_1_SLOT_LENGTH_US       5 
#define DS18B20_TM_READ_FULL_SLOT_LENGTH_US     80
#define DS18B20_TM_READ_INIT_LENGTH_US          5
#define DS18B20_TM_READ_SAMPLE_AFTER_INIT_US    10
#define DS18B20_TM_RELEASE_SLOT_US              5

#define DS18B20_MAX_CONVERSION_TIME_MS          750

class TempSensor {
public:
  void initDriver();
  void setResolution(unsigned char resolution);
  int readTemperature();
  int isExternalPowerSupply();
private:
  int initProc();
  int skipRom();
  void writeStratchpad(unsigned char* data);
  void readStratchpad(unsigned char* data);
  void copyStratchpad();
  void recallE();
  int readPowerSupply();
  void convertT();
  void writeByte(unsigned char byte);
  unsigned char readByte();
  int waitDQ();
  void writeSlot(unsigned char bit);
  unsigned char readSlot();
  void release();
  void pullLow();
};


#endif /* TEMP-SENSOR_H_ */
