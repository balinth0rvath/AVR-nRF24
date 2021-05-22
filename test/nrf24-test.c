#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "nrf24-lib.h"
#include "nrf24-test-cases.h"
 
int main(int argc, char *argv[])
{
	char message[50] = "";
	int fd=0;
	int ret=0;
	char c_out[64]= {};
	char c_in[64]= {};
	int count,i;
	char opt = *argv[1];
	int fail = 0;

	ret = nrf24_test_open(&fd, message);
	printf("%s", message);	
	
	switch(opt)
	{
		case 't':
			ret = nrf24_lib_set_transmitter(fd);
			printf("transmitter set: %i\n", ret);
			break;

		case 'e':
			ret = nrf24_lib_set_receiver(fd);		
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
			ret = nrf24_lib_set_transmitter(fd);
			c_out[0] = 1;
			c_out[1] = 2;
			c_out[2] = 3;
			c_out[3] = 100;
			c_out[4] = 0;
			count = write(fd, c_out, 5);
			ret = nrf24_lib_set_receiver(fd);
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
