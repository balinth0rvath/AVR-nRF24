#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "nrf24-lib.h"
#include "nrf24-test-steps.h"
#include "nrf24-test-cases.h"
 
int main(int argc, char *argv[])
{
	char message[ERROR_MESSAGE_SIZE] = "";
	int fd=0;
	int ret=0;
	int error=0;
	char opt = 0;
	
	if (argc!=1)
	{
		opt = *argv[1];
	}
	
	ret = nrf24_test_open(&fd, message);
	printf("%s", message);	

	printf("*** Selected test ***\n");
	switch(opt)
	{
		case '1':
			printf("---------------------\n");
			printf("| Test long loop    |\n");
			printf("---------------------\n");
			ret = test_longloop(&fd, &error, message);
			break;
		case '2':
			printf("---------------------\n");
			printf("| Test addresses    |\n");
			printf("---------------------\n");
			ret = test_addresses(&fd, &error, message);
			break;
		default:
			printf("---------------------\n");
			printf("| No test selected  |\n");
			printf("---------------------\n");
			break;
	}

	ret |= nrf24_test_close(&fd, message);
	printf("%s", message);	
	usleep(SLEEP_US);

	printf("Number of errors: %i\n", error);
	if (!ret)
	{
		printf("ALL TEST STEPS PASSED\n");
	} else
	{
		printf("**** TEST FAILED\n");
	}	
	return 0;
}
