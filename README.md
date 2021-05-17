## Setup
Host: 
$ . /opt/poky/3.2.2/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi
$ make 
$ scp nrf24.ko root@192.168.1.137:/home/root/.
$ scp nrf24-test root@192.168.1.137:/home/root/.

Rpi:
$ insmod nrf24.ko
$ ./nrf24-test '1' 

20200517
nRF24L01driver:8a4c0cdf7803df328cc4730b5077f52fee5dc932
AVR-nRF24@master:3eb939d17d8ff22d65403e109d810f7f85f86c85
