/*
 * ds18b20.c
 *
 * Created: 8/31/2021 4:19:00 PM
 *  Author: bhorvath2
 */ 

#include "common.h"
#include <avr/io.h>
#include <util/delay.h>
#include "ds18b20.h"

static void ds18b20_release(void);
static int ds18b20_wait_dq(void);
static int ds18b20_init_proc(void);

int ds18b20_init_driver(void)
{  
  ds18b20_release();
  return ds18b20_init_proc();
}

static void ds18b20_release(void)
{
  DS18B20_DDR &= ~(1 << DS18B20_DQ);           // DQ is input and
  DS18B20_PORT &= ~(1 << DS18B20_DQ);          // no pull up. A 4.7k resistor pulls it up
}

static int ds18b20_wait_dq(void)
{
  int ret = -1;
  int i = DS18B20_TM_PRESENCE_COUNT;  
  while(i)                                    // Searching for presence pulse
  {
    if (  (DS18B20_PIN & (1 << DS18B20_DQ)) == 0)
    {
      ret = 0;                                // Presence pulse found      
    }
    i--;
    _delay_us(DS18B20_TM_PRESENCE_SLICE_US);  // fix delay is DS18B20_TM_PRESENCE_SLICE_US *
  }                                           // DS18B20_TM_PRESENCE_COUNT
  volatile b = ret;    
  return ret;
}

static int ds18b20_init_proc(void)
{
  int ret = 0;

  DS18B20_DDR |= 1 << DS18B20_DQ;             // DQ is output
  DS18B20_PORT &= ~(1 << DS18B20_DQ);         // Pull DQ low for at least 480us
  
  _delay_us(DS18B20_TM_RESET_PULSE_US);
  ds18b20_release();
  _delay_us(DS18B20_TM_WAIT_RESET_US);
  ret = ds18b20_wait_dq();
  _delay_us(DS18B20_TM_WAIT_PRESENCE_US);     // wait 200us after the 300us. 480us is the minimum
  return ret;
}


void ds18b20_set_resolution(uint8_t resolution)
{

}

int ds18b20_read_temperature()
{
  return 0;
}