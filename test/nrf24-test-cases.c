#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include "nrf24-lib.h"
#include "nrf24-test-cases.h"

char g_c_out[64]= {1,2,3,100,0};
char g_c_in[64]= {};

int nrf24_test_open(int* fd, char* message)
{
 	char* message_fail = "Open driver: failed\n";
	char* message_pass = "Open driver: passed\n";
	*fd = nrf24_lib_open(); 
	if (*fd==0)
	{
		strncpy(message, message_fail, ERROR_MESSAGE_SIZE);
		return -1;
	}
	strncpy(message, message_pass, ERROR_MESSAGE_SIZE);
	return 0;
}

int nrf24_test_close(int* fd, char* message)
{
	int ret = 0;
 	char* message_fail = "Close driver: failed\n";
	char* message_pass = "Close driver: passed\n";
	ret = close(*fd); 
	if (ret!=0)
	{
		strncpy(message, message_fail, ERROR_MESSAGE_SIZE);
		return -1;
	}
	strncpy(message, message_pass, ERROR_MESSAGE_SIZE);
	return 0;

}

int nrf24_test_set_receiver(int* fd, char* message)
{
	int ret = 0;
 	char* message_fail = "Set RX mode: failed\n";
	char* message_pass = "Set RX mode: passed\n";

	ret = nrf24_lib_set_receiver(*fd);
	if (ret)
	{
		strncpy(message,message_fail, ERROR_MESSAGE_SIZE);
		return -1;
	} 
	strncpy(message, message_pass, ERROR_MESSAGE_SIZE);
	return 0;
}

int nrf24_test_set_transmitter(int* fd, char* message)
{
	int ret = 0;
 	char* message_fail = "Set TX mode: failed\n";
	char* message_pass = "Set TX mode: passed\n";
	ret = nrf24_lib_set_transmitter(*fd);
	if (ret)
	{
		strncpy(message,message_fail, ERROR_MESSAGE_SIZE);
		return -1;
	} 
	strncpy(message, message_pass, ERROR_MESSAGE_SIZE);
	return 0;
}

int nrf24_test_matching_address(int* fd, char* message)
{
 	int ret = 0;
	unsigned char target_address[5] = TARGET_MATCHING_ADDRESS;
 	char* message_fail = "Set matching address: failed\n";
	char message_pass[ERROR_MESSAGE_SIZE] = "";
	unsigned char octets[5] = TARGET_MATCHING_ADDRESS;
	sprintf(message_pass,"Set matching address: passed %x:%x:%x:%x:%x \n",
		target_address[0],
		target_address[1],
		target_address[2],
		target_address[3],
		target_address[4]
	); 
	ret = nrf24_lib_set_address(*fd, octets);
	if (ret)
	{
		strncpy(message,message_fail, ERROR_MESSAGE_SIZE);
		return -1;
	} 
	strncpy(message, message_pass, ERROR_MESSAGE_SIZE);
	return 0;
}

int nrf24_test_different_address(int* fd, char* message)
{
 	int ret = 0;
 	char* message_fail = "Set different address: failed\n";
	char* message_pass = "Set different address: passed (1,2,3,4,5)\n";
	unsigned char octets[5] = TARGET_BAD_ADDRESS;
	ret = nrf24_lib_set_address(*fd, octets);
	if (ret)
	{
		strncpy(message,message_fail, ERROR_MESSAGE_SIZE);
		return -1;
	} 
	strncpy(message, message_pass, ERROR_MESSAGE_SIZE);
	return 0;
}

int nrf24_test_send_data(int* fd, char* message)
{
	int count = 0;
 	char* message_fail = "Send data: failed\n";
	char* message_pass = "Send data: passed\n";
	count = write(*fd, g_c_out, 5);
	if (count !=5)
	{
		strncpy(message,message_fail,ERROR_MESSAGE_SIZE);
		return -1;
	} 
	strncpy(message,message_pass,ERROR_MESSAGE_SIZE);
	return 0;
}

int nrf24_test_receive_data(int* fd, char* message)
{
	int ret = 0;
	int count = 0;
	int i = 0;
 	char* message_fail_size = "Receive data: failed (different sized payload receive)\n";
 	char* message_fail_diff = "Receive data: failed (different payload received)\n";
	char* message_pass = "Receive data: passed\n";
	count = read(*fd, g_c_in, 64);
	if (count !=8)
	{
		strncpy(message,message_fail_size,ERROR_MESSAGE_SIZE);
		return -1;
	}			
	for(i=0;i<4;i++)
	{
		if (g_c_in[i]!=g_c_out[i])
		{
			strncpy(message,message_fail_diff,ERROR_MESSAGE_SIZE);
			return -1;
		}
	}
	strncpy(message,message_pass,ERROR_MESSAGE_SIZE);
	return 0;
}

int nrf24_test_receive_data_fail(int* fd, char* message)
{
	int ret = 0;
 	char* message_fail = "Receive data using bad address: failed\n";
	char* message_pass = "Receive data using bad address: passed\n";
	ret = nrf24_test_receive_data(fd, message);
	if (!ret)
	{
		strncpy(message,message_fail, ERROR_MESSAGE_SIZE);
		return -1;
	} 
	strncpy(message, message_pass, ERROR_MESSAGE_SIZE);
	return 0;

	
}
