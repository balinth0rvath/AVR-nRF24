#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "nrf24-lib.h"

struct ADDRESS_T {
	unsigned char octet[5];
};

#define NRF24_IOCTL_SET_RECEIVER _IO(153,0)
#define NRF24_IOCTL_SET_TRANSMITTER _IO(153,1)
#define NRF24_IOCTL_SET_ADDRESS _IOW(153,2, struct ADDRESS_T)

void nrf24_lib_func(void)
{

}
int nrf24_lib_open()
{
	int fd = 0;
	fd = open("/dev/nrf24d", O_RDWR);
	return fd;
}
	
int nrf24_lib_set_transmitter(int fd)
{
	int ret = 0;
	ret = ioctl(fd, NRF24_IOCTL_SET_TRANSMITTER, 0);		
	return ret;
}

int nrf24_lib_set_receiver(int fd)
{
	int ret = 0;
	ret = ioctl(fd, NRF24_IOCTL_SET_RECEIVER, 0);		
	return ret;
}

int nrf24_lib_set_address(int fd, unsigned char* octets)
{
	int i=0;	
	int ret = 0;
	struct ADDRESS_T address;
	for(int i=0;i<5;i++)
	{
		address.octet[i] = *(octets + i);	
	}
	ret = ioctl(fd, NRF24_IOCTL_SET_ADDRESS, (void*)&address);
	return ret;
}
