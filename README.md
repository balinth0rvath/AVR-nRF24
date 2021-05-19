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
