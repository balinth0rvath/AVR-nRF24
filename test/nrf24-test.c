#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "nrf24-lib.h"
#include "nrf24-test-cases.h"

#define SLEEP_S 1 
 
int main(int argc, char *argv[])
{
	char message[ERROR_MESSAGE_SIZE] = "";
	int fd=0;
	int ret=0;
	char opt = *argv[1];

	ret = nrf24_test_open(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_matching_address(&fd, message);
	printf("%s", message);
	sleep(SLEEP_S);

	ret |= nrf24_test_set_transmitter(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_send_data(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_set_receiver(&fd, message);
	printf("%s", message);	
	sleep(SLEEP_S);

	ret |= nrf24_test_receive_data(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_different_address(&fd, message);
	printf("%s", message);

	ret |= nrf24_test_set_transmitter(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_send_data(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_set_receiver(&fd, message);
	printf("%s", message);	
	sleep(SLEEP_S);

	ret |= nrf24_test_receive_data_fail(&fd, message);
	printf("%s", message);	


	ret |= nrf24_test_matching_address(&fd, message);
	printf("%s", message);
	sleep(SLEEP_S);

	ret |= nrf24_test_set_transmitter(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_send_data(&fd, message);
	printf("%s", message);	

	ret |= nrf24_test_set_receiver(&fd, message);
	printf("%s", message);	
	sleep(SLEEP_S);

	nrf24_test_receive_data(&fd, message);


	ret |= nrf24_test_close(&fd, message);
	printf("%s", message);	
	sleep(SLEEP_S);

	if (!ret)
	{
		printf("ALL TESTS PASSED\n");
	} else
	{
		printf("**** TEST FAILED\n");
	}	
	return 0;
}
