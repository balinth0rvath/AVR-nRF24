CXXFLAGS = -O2 -pipe -g -feliminate-unused-debug-types
DEPS=wireless-device.h
LIB_NAME=libwireless.a
OBJ=wireless-device.o
ARFLAGS= -cvq

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(LIB_NAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm $(OBJ) $(LIB_NAME)

