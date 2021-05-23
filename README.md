# RF Communication Mesh
### Server
RPI Yocto using forked Kernel, custom nRF24L01 driver located in nRF24L01driver branch:<br>
https://github.com/balinth0rvath/linux<br>
Unit tests, test library<br> 
https://github.com/balinth0rvath/AVR-nRF24/test<br> 
<br>
<br>
### Sensor modules
ATmega firmware<br> 
https://github.com/balinth0rvath/AVR-nRF24/AVR-nRF24<br> 
<br>
<br>
## Proto1
Server <--> MCU1 <--> MCU2<br>
Server <-bitbang SPI-> nRF24L01<br>
MCU1 <-bitbang SPI-> nRF24L01<br>
MCU1 <-HW SPI-> nRF24L01<br>
Server <-Ethernet-> Host<br>
MCUx <-Atmel ICE SPI-> Host<br>

<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/photo_proto1.JPG" width="1024" /> 
</p>
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/schematics_proto1.png" width="1024" /> 
</p>


## Setup
Host:<br> 
$ . /opt/poky/3.2.2/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi<br>
$ make<br> 
$ scp nrf24.ko root@192.168.1.137:/home/root/.<br>
$ scp nrf24-test root@192.168.1.137:/home/root/.<br>

Rpi:<br>
$ insmod nrf24.ko<br>
$ ./nrf24-test '1'<br> 

20200517<br>
linux@nRF24L01driver:8a4c0cdf7803df328cc4730b5077f52fee5dc932<br>
AVR-nRF24@master:3eb939d17d8ff22d65403e109d810f7f85f86c85<br>
