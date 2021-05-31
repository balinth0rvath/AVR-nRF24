#define ERROR_MESSAGE_SIZE 64
#define TARGET_MATCHING_ADDRESS {0x10, 0x20, 0x30, 0x40, 0x50};
//#define TARGET_MATCHING_ADDRESS {0xe7, 0xe7, 0xe7, 0xe7, 0xe1};
#define TARGET_BAD_ADDRESS {1,2,3,4,5};

int nrf24_test_open(int* fd, char* message);
int nrf24_test_close(int* fd, char* message);
int nrf24_test_set_receiver(int* fd, char* message);
int nrf24_test_set_transmitter(int* fd, char* message);
int nrf24_test_matching_address(int* fd, char* message);
int nrf24_test_different_address(int* fd, char* message);
int nrf24_test_send_data(int* fd, char* message);
int nrf24_test_receive_data(int* fd, char* message);
int nrf24_test_receive_data_fail(int* fd, char* message);
