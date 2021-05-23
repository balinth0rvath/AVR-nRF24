#define ERROR_MESSAGE_SIZE 64

int nrf24_test_open(int* fd, char* message);
int nrf24_test_close(int* fd, char* message);
int nrf24_test_set_receiver(int* fd, char* message);
int nrf24_test_set_transmitter(int* fd, char* message);
int nrf24_test_send_data(int* fd, char* message);
int nrf24_test_receive_data(int* fd, char* message);
