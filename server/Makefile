CXXFLAGS = -O2 -pipe -g -feliminate-unused-debug-types
DEPS = controller.h
OBJ_SERVER = server.o controller.o 
OBJ_TEST = test.o controller.o 
LIBDIR=./lib/
LIBFLAG=-lwireless
LIBOBJ=$(LIBDIR)libwireless.a
LIBHEADER=$(LIBDIR)wireless-device.h
LIBSRC=$(LIBDIR)wireless-device.cpp

%.o: %.cpp $(DEPS) $(LIBDIR)$(LIBHEADER)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

server: $(OBJ_SERVER) 
	$(CXX) -o $@ $^ $(CXXFLAGS) -L$(LIBDIR) $(LIBFLAG)

test: $(OBJ_TEST) 
	$(CXX) -o $@ $^ $(CXXFLAGS) -L$(LIBDIR) $(LIBFLAG)

$(LIBOBJ): $(LIBSRC) $(LIBHEADER)
	cd $(LIBDIR); make;
clean:
	rm server test *.o 
