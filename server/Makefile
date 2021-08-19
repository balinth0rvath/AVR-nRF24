DEPS = controller.h wireless-device.h
OBJ = server.o wireless-device.o controller.o 

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

server: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm server *.o 
