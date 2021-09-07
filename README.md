# RF Communication Mesh
### Server
RPI Yocto using forked Kernel, custom nRF24L01 driver located in nRF24L01driver branch:<br>
https://github.com/balinth0rvath/linux<br>
Readme: https://github.com/balinth0rvath/linux/blob/nRF24L01driver/MEMO.md<br>
<br>
Unit tests, test library<br> 
https://github.com/balinth0rvath/AVR-nRF24/tree/master/test<br> 
Server app<br>
https://github.com/balinth0rvath/AVR-nRF24/tree/master/server<br> 
<br>
<br>
### Sensor modules
ATmega firmware<br> 
https://github.com/balinth0rvath/AVR-nRF24/tree/master/AVR-nRF24<br> 
<br>
<br>
## Proto1
Server <--> MCU1 <--> MCU2<br>
Server <-bitbang SPI-> nRF24L01<br>
MCU1 <-bitbang SPI-> nRF24L01<br>
MCU2 <-HW SPI-> nRF24L01<br>
Server <-Ethernet-> Host<br>
MCUx <-Atmel ICE SPI-> Host<br>
<br>
<br>
Server UML
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/server_uml.png" width="512" /> 
</p>
<br>
Schematics
<br>
<br>
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/schematics.png" width="512" /> 
</p>
<br>
Breadboard
<br>
<br>
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/photo_proto1.JPG" width="512" /> 
</p>
<br>
First prototype without sensors
<br>
<br>
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/pcb1.jpg" width="256" /> 
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/pcb2.jpg" width="256" /> 
</p>
<br>
DS18B20 Sequence of determining external power supply mode
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/read_power_supply_sequence.png" width="512" /> 
</p>

## Setup
Server app:<br> 
$ cd server<br>
$ . /opt/poky/3.2.2/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi<br>
$ make<br> 
$ scp server root@192.168.1.137:/home/root/.<br>

Test app:<br> 
$ cd test<br>
$ . /opt/poky/3.2.2/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi<br>
$ make<br> 
$ scp nrf24-test root@192.168.1.137:/home/root/.<br>


Driver:<br>
$ . /opt/poky/3.2.2/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi<br>
$ make<br> 
$ scp nrf24.ko root@192.168.1.137:/home/root/.<br>

Rpi:<br>
$ insmod nrf24.ko<br>
$ ./nrf24-test<br> 
$ ./server

MCU:<br>
Ext. Full-swing Crystal; Start-up time PWRDWN/RESET: 16K CK/14 CK + 65 ms

## Remote debug server

target:<br>
$ gdbserver :1234 server

host:<br>
$ arm-poky-linux-gnueabi-gdb server
(gdb) target remote 192.168.1.137:1234


## Release commits
20210517<br>
linux@nRF24L01driver:8a4c0cdf7803df328cc4730b5077f52fee5dc932<br>
AVR-nRF24@master:3eb939d17d8ff22d65403e109d810f7f85f86c85<br>

20210523<br>
linux@nRF24L01driver:8a4c0cdf7803df328cc4730b5077f52fee5dc932<br>
AVR-nRF24@master:7b25dc61fc849677e8bb6358456cee5dc74c7301<br>

20210531<br>
linux@nRF24L01driver:7c86b9ca6b2d7ad1549c76c9caeeb97bab567bfb<br>
AVR-nRF24@master:dd0c3b836620fbdc131a8c8d285148fa6ca5f2e6<br>

20210820<br>
linux@nRF24L01driver:7c86b9ca6b2d7ad1549c76c9caeeb97bab567bfb<br>
AVR-nRF24@master:5c61a1c3e08124cf29ecca4751b73a7c155e9e8<br>

