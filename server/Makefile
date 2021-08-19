DEPS = nrf24-controller.h 
OBJ = nrf24-server.o nrf24-controller.o 

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

nrf24-server: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm nrf24-server nrf24-controller.o 
