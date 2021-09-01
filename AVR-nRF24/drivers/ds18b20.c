/*
 * ds18b20.c
 *
 * Created: 8/31/2021 4:19:00 PM
 *
 * author: balinth0rvath 
 */ 

#include "common.h"
#include <avr/io.h>
#include <util/delay.h>
#include "ds18b20.h"


static int ds18b20_init_proc(void);
static int ds18b20_skip_rom(void);
static int ds18b20_write_stratchpad(uint8_t* data);
static int ds18b20_read_stratchpad(uint8_t* data);
static int ds18b20_copy_stratchpad(void);
static int ds18b20_recall_e(void);
static int ds18b20_read_power_supply(uint8_t* data);
static int ds18b20_convert_t(void);

static void ds18b20_write_byte(uint8_t byte);
static uint8_t ds18b20_read_byte();

static int ds18b20_wait_dq(void);
static void ds18b20_write_slot(uint8_t bit);
static uint8_t ds18b20_read_slot(void);
static void ds18b20_release(void);
static void ds18b20_pull_low(void);

int ds18b20_init_driver(void)
{  
  ds18b20_release();
  return ds18b20_init_proc();
}

void ds18b20_set_resolution(uint8_t resolution)
{
  
}

int ds18b20_read_temperature()
{
  int ret = 0;
  int data[9];
  ret = ds18b20_skip_rom();
  if (ret==-1)
  {
    return ret;
  }
  ret = ds18b20_convert_t();
  if (ret==-1)
  {
    return ret;
  }
  _delay_ms(DS18B20_MAX_CONVERSION_TIME_MS);

  ret = ds18b20_read_stratchpad(&data[0]);
  if (ret==-1)
  {
    return ret;
  }
  
  int temp_low = data[DS18B20_REG_TEMP_LSB];
  int temp_high = data[DS18B20_REG_TEMP_MSB]; 
  return (temp_low | (temp_high << 8));
}

static int ds18b20_init_proc(void)
{
  int ret = 0;

  ds18b20_pull_low();                         // Pull DQ low for at least 480us  
  _delay_us(DS18B20_TM_RESET_PULSE_US);
  ds18b20_release();                          // release and 
  _delay_us(DS18B20_TM_WAIT_RESET_US);        // wait for device to
  ret = ds18b20_wait_dq();                    // pull it low
  _delay_us(DS18B20_TM_WAIT_PRESENCE_US);     // wait 200us after the 300us. 480us is the minimum
  return ret;
}

static int ds18b20_skip_rom(void)
{
  int ret = ds18b20_init_proc();
  if (ret !=-1)
  {
    ds18b20_write_byte(DS18B20_CMD_SKIP_ROM);
  }
  return ret;
}

static int ds18b20_write_stratchpad(uint8_t* data)
{
  int ret = ds18b20_init_proc();
  if (ret !=-1)
  {
    ds18b20_write_byte(DS18B20_CMD_WRITE_SCRATCHPAD);
    for(int i=0;i<3;i++)
    {
      ds18b20_write_byte(*(data+i));  
    }
  }
  return ret;
}

static int ds18b20_read_stratchpad(uint8_t* data)
{
  int ret = ds18b20_init_proc();
  if (ret !=-1)
  {
    ds18b20_write_byte(DS18B20_CMD_READ_SCRATCHPAD);
    for(int i=0;i<9;i++)
    {
      *(data+i) = ds18b20_read_byte();
    }
  }   
  return ret; 
}

static int ds18b20_copy_stratchpad(void)
{
  int ret = ds18b20_init_proc();
  if (ret !=-1)
  {
    ds18b20_write_byte(DS18B20_CMD_COPY_SCRATCHPAD);
  }
  return ret;
}

static int ds18b20_recall_e(void)
{
  int ret = ds18b20_init_proc();
  if (ret !=-1)
  {
    ds18b20_write_byte(DS18B20_CMD_RECALL_E);
  }
  return ret;
}

static int ds18b20_read_power_supply(uint8_t* data)
{
  return 0;
}

static int ds18b20_convert_t(void)
{
  int ret = ds18b20_init_proc();
  if (ret !=-1)
  {
    ds18b20_write_byte(DS18B20_CMD_CONVERT_T);
  }
  return ret;
}

static void ds18b20_write_byte(uint8_t byte)
{
  for(int i=0; i<8; i++)
  {
    ds18b20_write_slot((1 << i) & byte);  
  }
}

static uint8_t ds18b20_read_byte()
{
  uint8_t ret = 0;
  uint8_t bit = 0;
  for(int i=0; i<8; i++)
  {
    bit = ds18b20_read_slot();
    ret+=(bit << i);
  }
  return ret;
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
  return ret;
}

static void ds18b20_write_slot(uint8_t bit)
{
  if (!bit)
  {
    ds18b20_pull_low();                             // write 0 slot
    _delay_us(DS18B20_TM_WRITE_FULL_SLOT_LENGTH_US);
    ds18b20_release();
    _delay_us(DS18B20_TM_RELEASE_SLOT_US);
  } else                                            // write 1 slot
  {
    ds18b20_pull_low();
    _delay_us(DS18B20_TM_WRITE_1_SLOT_LENGTH_US);
    ds18b20_release();
    _delay_us(DS18B20_TM_WRITE_FULL_SLOT_LENGTH_US -
    DS18B20_TM_WRITE_1_SLOT_LENGTH_US +
    DS18B20_TM_RELEASE_SLOT_US);
  }
  
}

static uint8_t ds18b20_read_slot(void)
{
  int sample = 0;
  ds18b20_pull_low();
  _delay_us(DS18B20_TM_READ_INIT_LENGTH_US);
  ds18b20_release();
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

static void ds18b20_release(void)
{
  DS18B20_DDR &= ~(1 << DS18B20_DQ);           // DQ is input and
  DS18B20_PORT &= ~(1 << DS18B20_DQ);          // no pull up. A 4.7k resistor pulls it up
}

static void ds18b20_pull_low(void)
{
  DS18B20_DDR |= 1 << DS18B20_DQ;              // DQ is output and
  DS18B20_PORT &= ~(1 << DS18B20_DQ);          // set to 0
}