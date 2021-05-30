#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "nrf24-lib.h"
#include "nrf24-test-cases.h"

#define SLEEP_US 1000000 
 
int main(int argc, char *argv[])
{
	char message[ERROR_MESSAGE_SIZE] = "";
	int fd=0;
	int ret=0;
 	int i=0;
	int error=0;
	char opt = *argv[1];

	ret = nrf24_test_open(&fd, message);
	printf("%s", message);	
/*
	ret |= nrf24_test_matching_address(&fd, message);
	printf("%s", message);
	usleep(SLEEP_US);
*/
	for (i=0;i<10;i++)
	{
		ret |= nrf24_test_set_transmitter(&fd, message);
		printf("%s", message);	

		ret |= nrf24_test_send_data(&fd, message);
		printf("%s", message);	

		ret |= nrf24_test_set_receiver(&fd, message);
		printf("%s", message);	
		usleep(SLEEP_US);

		ret |= nrf24_test_receive_data(&fd, message);
		printf("%s", message);	
		usleep(SLEEP_US);
		printf("%i",i);
		if (ret)
		{
			error++;
			ret = 0;
		}
	}
/*
	ret |= nrf24_test_different_address(&fd, message);
	printf("%s", message);
	ret |= nrf24_test_set_transmitter(&fd, message);
	printf("%s", message);	
	usleep(SLEEP_US);

	ret |= nrf24_test_send_data(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_set_receiver(&fd, message);
	printf("%s", message);	
	usleep(SLEEP_US);

	ret |= nrf24_test_receive_data(&fd, message);
	printf("%s", message);	
	ret |= nrf24_test_matching_address(&fd, message);
	printf("%s", message);
	usleep(SLEEP_US);
	ret |= nrf24_test_set_transmitter(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_send_data(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_set_receiver(&fd, message);
	printf("%s", message);	
	usleep(SLEEP_US);

	ret |= nrf24_test_receive_data(&fd, message);
	printf("%s", message);	
*/
	ret |= nrf24_test_close(&fd, message);
	printf("%s", message);	
	usleep(SLEEP_US);

	printf("Number of errors: %i\n", error);
	if (!ret)
	{
		printf("ALL TESTS PASSED\n");
	} else
	{
		printf("**** TEST FAILED\n");
	}	
	return 0;
}
