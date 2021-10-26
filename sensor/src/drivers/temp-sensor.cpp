/*
 * temp_sensor.cpp
 *
 * Created: 10/26/2021 5:43:59 PM
 *  Author: balinth0horvath
 */ 

 #include "common.h"
 #include <avr/io.h>
 #include <util/delay.h>
 #include "temp-sensor.h"

 void TempSensor::initDriver()
 {
   release();
 }

 void TempSensor::setResolution(unsigned char resolution)
 {
   
 }

 int TempSensor::readTemperature()
 {
   uint8_t data[9];
   int ret = skipRom();
   if (ret==-1)
   {
     return ret;
   }
   convertT();
   
   _delay_ms(DS18B20_MAX_CONVERSION_TIME_MS);
   ret = skipRom();
   if (ret==-1)
   {
     return ret;
   }
   readStratchpad(&data[0]);
   
   uint8_t temp_low = data[DS18B20_REG_TEMP_LSB];
   uint8_t temp_high = data[DS18B20_REG_TEMP_MSB];
   return (temp_low | (temp_high << 8));   
 }

 int TempSensor::isExternalPowerSupply()
 {
   int ret = skipRom();
   if (ret==-1)
   {
     return ret;
   }
   return readPowerSupply();   
 }

int TempSensor::initProc()
{
  int ret = 0;
  pullLow();                                          // Pull DQ low for at least 480us
  _delay_us(DS18B20_TM_RESET_PULSE_US);
  release();                                          // release and
  _delay_us(DS18B20_TM_WAIT_RESET_US);                // wait for device to
  ret = waitDQ();                                     // pull it low
  _delay_us(DS18B20_TM_WAIT_PRESENCE_US);             // wait 200us after the 300us. 480us is the minimum
  return ret;
}

int TempSensor::skipRom()
{
  int ret = initProc();
  if (ret !=-1)
  {
    writeByte(DS18B20_CMD_SKIP_ROM);
  }
  return ret;
}

void TempSensor::writeStratchpad(unsigned char* data)
{
  writeByte(DS18B20_CMD_WRITE_SCRATCHPAD);
  for(int i=0;i<3;i++)
  {
    writeByte(*(data+i));
  }
}

void TempSensor::readStratchpad(unsigned char* data)
{
  writeByte(DS18B20_CMD_READ_SCRATCHPAD);
  for(int i=0;i<9;i++)
  {
    *(data+i) = readByte();
  }
}

void TempSensor::copyStratchpad()
{
  writeByte(DS18B20_CMD_COPY_SCRATCHPAD);
}

void TempSensor::recallE()
{
  writeByte(DS18B20_CMD_RECALL_E);  
}

int TempSensor::readPowerSupply()
{
  uint8_t ret = 0;
  writeByte(DS18B20_CMD_READ_POWER_SUPPLY);
  ret = readSlot();
  return ret != 0;
}

void TempSensor::convertT()
{
  writeByte(DS18B20_CMD_CONVERT_T);
}

void TempSensor::writeByte(unsigned char byte)
{
  for(int i=0; i<8; i++)
  {
    writeSlot((1 << i) & byte);
  }
}

unsigned char TempSensor::readByte()
{
  unsigned char ret = 0;
  unsigned char bit = 0;
  for(int i=0; i<8; i++)
  {
    bit = readSlot();
    ret+=(bit << i);
  }
  return ret;
}

int TempSensor::waitDQ()
{
  int ret = -1;
  int i = DS18B20_TM_PRESENCE_COUNT;
  while(i)                                              // Searching for presence pulse
  {
    if (  (DS18B20_PIN & (1 << DS18B20_DQ)) == 0)
    {
      ret = 0;                                          // Presence pulse found
    }
    i--;
    _delay_us(DS18B20_TM_PRESENCE_SLICE_US);            // fix delay is DS18B20_TM_PRESENCE_SLICE_US *
  }                                                     // DS18B20_TM_PRESENCE_COUNT
  return ret;
}

void TempSensor::writeSlot(unsigned char bit)
{
  if (!bit)
  {
    pullLow();                                          // write 0 slot
    _delay_us(DS18B20_TM_WRITE_FULL_SLOT_LENGTH_US);
    release();
    _delay_us(DS18B20_TM_RELEASE_SLOT_US);
  } else                                                // write 1 slot
  {
    pullLow();
    _delay_us(DS18B20_TM_WRITE_1_SLOT_LENGTH_US);
    release();
    _delay_us(DS18B20_TM_WRITE_FULL_SLOT_LENGTH_US -
    DS18B20_TM_WRITE_1_SLOT_LENGTH_US +
    DS18B20_TM_RELEASE_SLOT_US);
  }
}

unsigned char TempSensor::readSlot()
{
  int sample = 0;
  pullLow();
  _delay_us(DS18B20_TM_READ_INIT_LENGTH_US);
  release();
  _delay_us(DS18B20_TM_READ_SAMPLE_AFTER_INIT_US);
  if ((DS18B20_PIN & (1 << DS18B20_DQ))!=0)
  {
    sample = 1;
  } else
  {
    sample = 0;
  }
  _delay_us(DS18B20_TM_READ_FULL_SLOT_LENGTH_US -
  DS18B20_TM_READ_INIT_LENGTH_US -
  DS18B20_TM_READ_SAMPLE_AFTER_INIT_US +
  DS18B20_TM_RELEASE_SLOT_US);

  return sample;
}

void TempSensor::release()
{
  DS18B20_DDR &= ~(1 << DS18B20_DQ);                    // DQ is input and
  DS18B20_PORT &= ~(1 << DS18B20_DQ);                   // no pull up. A 4.7k resistor pulls it up
}

void TempSensor::pullLow()
{
  DS18B20_DDR |= 1 << DS18B20_DQ;                       // DQ is output and
  DS18B20_PORT &= ~(1 << DS18B20_DQ);                   // set to 0
}


