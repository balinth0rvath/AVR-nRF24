#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "nrf24-lib.h"

#define NRF24_IOCTL_SET_RECEIVER _IO(153,0)
#define NRF24_IOCTL_SET_TRANSMITTER _IO(153,1)

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
