#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "nrf24-lib.h"
#include "nrf24-test-steps.h"
#include "nrf24-test-cases.h"

/*********************************************************
	Test long loop
  --------------
	Sends one packet, then receives two packets 1000 times
	One handshaking is ok, when the data sent is received
	two times with the same payload 
	Counts the number of handshakes with problems in error 
	variable

*********************************************************/
int test_longloop(int* fd, int* error, char* message)
{
	int i=0;
	int ret=0;

	ret |= nrf24_test_matching_address(fd, message);
	printf("%s", message);
	usleep(SLEEP_US);

	for (i=0;i<1000;i++)
	{
		ret |= nrf24_test_set_transmitter(fd, message);
		printf("%s", message);	

		ret |= nrf24_test_send_data(fd, message);
		printf("%s", message);	

		ret |= nrf24_test_set_receiver(fd, message);
		printf("%s", message);	
		usleep(SLEEP_US);

		ret |= nrf24_test_receive_data(fd, message);
		printf("%s", message);	
		usleep(SLEEP_US);
		printf("%i",i);
		if (ret)
		{
			*error++;
			ret = 0;
		}
	}
	return ret;
}

/*********************************************************
	Test change address
  --------------
	Sends one packet, then receives two packets 

*********************************************************/
int test_addresses(int* fd, int* error, char* message)
{
	int ret = 0;
	ret |= nrf24_test_matching_address(fd, message);
	printf("%s", message);
	usleep(SLEEP_US);

	ret |= nrf24_test_set_transmitter(fd, message);
	printf("%s", message);	
	usleep(SLEEP_US);

	ret |= nrf24_test_send_data(fd, message);
	printf("%s", message);	

	ret |= nrf24_test_set_receiver(fd, message);
	printf("%s", message);	
	usleep(SLEEP_US);

	ret |= nrf24_test_receive_data(fd, message);	
	printf("%s", message);	

	ret |= nrf24_test_different_address(fd, message);
	printf("%s", message);

	ret |= nrf24_test_set_transmitter(fd, message);
	printf("%s", message);	
	usleep(SLEEP_US);

	ret |= nrf24_test_send_data(fd, message);
	printf("%s", message);	

	ret |= nrf24_test_set_receiver(fd, message);
	printf("%s", message);	
	usleep(SLEEP_US);

	ret |= !nrf24_test_receive_data(fd, message);
	printf("%s", message);	

	return ret;
}

