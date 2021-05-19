DEPS = nrf24-lib.h nrf24-test-cases.h
OBJ = nrf24-test.o nrf24-lib.o nrf24-test-cases.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

nrf24-test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm nrf24-test nrf24-test.o nrf24-test-cases.o nrf24-lib.o
