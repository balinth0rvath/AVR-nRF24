#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "nrf24-lib.h"
#include "nrf24-test-cases.h"

#define NRF24_IOCTL_SET_RECEIVER _IO(153,0)
#define NRF24_IOCTL_SET_TRANSMITTER _IO(153,1)

 
int main(int argc, char *argv[])
{
	int fd;
	int ret=0;
	char c_out[64]= {};
	char c_in[64]= {};
	int count,i;
	char opt = *argv[1];

	int fail = 0;

	fd = open("/dev/nrf24d", O_RDWR);
	if (fd==0)
	{
		printf("cannot open driver\n");
		exit(1);
	}
	printf("driver opened\n");
	switch(opt)
	{
		case 't':
			ret = ioctl(fd, NRF24_IOCTL_SET_TRANSMITTER, 0);		
			printf("transmitter set: %i\n", ret);
			break;

		case 'e':
			ret = ioctl(fd, NRF24_IOCTL_SET_RECEIVER, 0);		
			printf("Receiver set: %i\n", ret);
			break;

		case 'w':
			c_out[0] = 3;
			c_out[1] = 1;
			c_out[2] = 4;
			c_out[3] = 100;
			count = write(fd, c_out, 4);
			printf("number of bytes wrote %i \n", count);
			break;

		case 'r':
			count = read(fd, c_in, 64);
			printf("number of bytes read %i \n", count);
			for(i=0;i<count;i++)
				printf("index %i value %i \n",i,c_in[i]);
			break;

		case '1':
			ret = ioctl(fd, NRF24_IOCTL_SET_TRANSMITTER, 0);		
			c_out[0] = 1;
			c_out[1] = 2;
			c_out[2] = 3;
			c_out[3] = 100;
			c_out[4] = 0;
			count = write(fd, c_out, 5);
			ret = ioctl(fd, NRF24_IOCTL_SET_RECEIVER, 0);		
			sleep(1);
			count = read(fd, c_in, 64);
			if (count !=8)
			{
				fail = 1;
			}			
			for(i=0;i<4;i++)
			{
				if (c_in[i]!=c_out[i])
				{
					fail = 1;
				}
			}
			if (fail)
			{
				printf("Test failed\n");
				printf("number of bytes read %i \n", count);
				for(i=0;i<count;i++)	
					printf("index %i value %i \n",i,c_in[i]);
			} else
			{
				printf("Test passed\n");
			}
			
		default:
			break;
	}

	close(fd);
	printf("driver closed\n");
	return 0;
}
