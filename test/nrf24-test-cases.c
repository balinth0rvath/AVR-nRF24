#include "nrf24-test-cases.h"
#include "nrf24-lib.h"
#include <string.h>

void tc_1(void)
{

}

int nrf24_test_open(int* fd, char* message)
{
 	char* message_fail = "Open driver: failed\n";
	char* message_pass = "Open driver: passed\n";
	*fd = nrf24_lib_open(); 
	if (*fd==0)
	{
		strncpy(message, message_fail, 50);
		return -1;
	}
	strncpy(message, message_pass, 50);
	return 0;
}

int nrf24_test_set_receiver(int* fd)
{
	int ret = 0;
	return ret;
}

int nrf24_test_set_transmitter(int* fd)
{
	int ret = 0;
	return 0;
}

int nrf24_test_duplex(int* fd)
{
	int ret = 0;
	return 0;
}
