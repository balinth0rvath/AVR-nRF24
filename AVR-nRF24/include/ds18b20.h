/*
 * ds18b20.h
 *
 * Created: 8/31/2021 4:19:25 PM
 */ 

#ifndef DS18B20_H_
#define DS18B20_H_

#include <avr/io.h>

// Data Port
#define DS18B20_DDR                     DDRB
#define DS18B20_PORT                    PORTB
#define DS18B20_PIN                     PINB

// Pin settings
#define DS18B20_DQ                      PB0

// DS18B20 commands
#define DS18B20_CMD_SEARCH_ROM          0xf0
#define DS18B20_CMD_READ_ROM            0x33
#define DS18B20_CMD_MATCH_ROM           0x55
#define DS18B20_CMD_SKIP_ROM            0xcc
#define DS18B20_CMD_ALARM_SEARCH        0xec
#define DS18B20_CMD_CONVERT_T           0x44
#define DS18B20_CMD_READ_SCRATCHPAD     0xbe
#define DS18B20_CMD_WRITE_SCRATCHPAD    0x4e
#define DS18B20_CMD_COPY_SCRATCHPAD     0x48
#define DS18B20_CMD_RECALL_E            0xb8
#define DS18B20_CMD_READ_POWER_SUPPLY   0xb4


// DS18B20 registers
#define DS18B20_REG_TEMP_LSB            0x00
#define DS18B20_REG_TEMP_MSB            0x01
#define DS18B20_REG_TH                  0x02
#define DS18B20_REG_TL                  0x03
#define DS18B20_REG_CONFIG              0x04
#define DS18B20_REG_CRC                 0x08

// DS18B20 timings
#define DS18B20_TM_RESET_PULSE_US       500
#define DS18B20_TM_WAIT_RESET_US        15
#define DS18B20_TM_PRESENCE_SLICE_US    20              // This one and one below means waiting for 300us with 20us sample period
#define DS18B20_TM_PRESENCE_COUNT       15
#define DS18B20_TM_WAIT_PRESENCE_US     200             

int ds18b20_init_driver(void);
void ds18b20_set_resolution(uint8_t resolution);
int ds18b20_read_temperature();

#endif /* DS18B20_H_ */