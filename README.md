# RF Communication Mesh
### What is This?
A sensor network consisting a server running on a Pi and several environmental sensors based on AVR MCU's. These things are communicating each other via RF and the sensors are planned to work through a season with one charge of a 3.6V battery.  

---

### Code Structure
|Module|Description|
|------|-----------|
|Custom nRF24L01 driver in my forked Linux repo located in nRF24L01driver branch|https://github.com/balinth0rvath/linux|
|Server app and unit tests|https://github.com/balinth0rvath/AVR-nRF24/tree/master/server|
|HW related stuff (Schematics, gerber files etc...)|https://github.com/balinth0rvath/AVR-nRF24/tree/master/nRF24-design|
|Firmware of a sensor module|https://github.com/balinth0rvath/AVR-nRF24/tree/master/AVR-nRF24|

---

### Server UML
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/server_uml.png" width="800" /> 
</p>

---

### Schematics
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/schematics.png" width="800" /> 
</p>

---

### Current Dev Setup (Mid of September 2021)
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/photo_dev_setup.jpg" width="800" />   
</p>

---

### Breadboard in Early Stage
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/photo_proto1.JPG" width="800" /> 
</p>

---

### First Prototypes without Sensors
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/pcb1.jpg" width="400" /> 
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/pcb2.jpg" width="400" /> 
</p>

---

### Debug Screenshot, DS18B20 Sequence of Determining External Power Supply Mode
<p float="left">  
  <img src="https://github.com/balinth0rvath/AVR-nRF24/blob/master/img/read_power_supply_sequence.png" width="800" /> 
</p>

---
## Setup
### Init Yocto Build
```
$ mkdir rpi
$ cd rpi
$ git clone -b gatesgarth git://git.yoctoproject.org/poky.git
$ git clone -b gatesgarth git://git.openembedded.org/meta-openembedded
$ git clone -b gatesgarth git://git.openembedded.org/meta-raspberrypi
$ source poky/oe-init-build-env ./build
```
./build/conf/bblayers.conf
```
# POKY_BBLAYERS_CONF_VERSION is increased each time build/conf/bblayers.conf
# changes incompatibly
POKY_BBLAYERS_CONF_VERSION = "2" 

BBPATH = "${TOPDIR}"
BBFILES ?= ""

BBLAYERS ?= " \ 
  ~/yocto-builds/rpi/poky/meta \
  ~/yocto-builds/rpi/poky/meta-poky \
  ~/yocto-builds/rpi/poky/meta-yocto-bsp \
  ~/yocto-builds/rpi/meta-raspberrypi \
  ~/yocto-builds/rpi/meta-openembedded/meta-oe \
  ~/yocto-builds/rpi/meta-openembedded/meta-python \
  ~/yocto-builds/rpi/meta-openembedded/meta-networking \
  ~/yocto-builds/rpi/meta-openembedded/meta-multimedia \
  ~/yocto-builds/rpi/meta-openembedded/meta-filesystems \
  "
```

./build/conf/local.conf
```
...
MACHINE ??= "raspberrypi3"
...
DISTRO_FEATURES_append = "bluez5 bluetooth wifi systemd"
VIRTUAL-RUNTIME_init_manager = "systemd"
IMAGE_INSTALL_append = "crda iw bluez5 wpa-supplicant openssh"
```

### Create SD Card Image and SDK
```
$ cd rpi
$ . poky/oe-init-build-env
$ bitbake core-image-minimal
$ bitbake core-image-minimal -c populate_sdk
$ tmp/deploy/sdk/poky-glibc-x86_64-core-image-minimal-cortexa7t2hf-neon-vfpv4-raspberrypi3-toolchain-3.2.2.sh
```

### Install Packages
```
$ sudo apt-get install libssl-dev
```

### Build Kernel
```
$ cd <linux source root>
$ . /opt/poky/3.2.2/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi 
$ KERNEL=kernel7
$ make ARCH=arm CROSS_COMPILE=arm-poky-linux-gnueabi- bcm2709_defconfig
$ make ARCH=arm CROSS_COMPILE=arm-poky-linux-gnueabi- zImage modules dtbs
```

### Build Driver
```
$ cd <linux source root>
$ . /opt/poky/3.2.2/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi 
$ make ARCH=arm CROSS_COMPILE=arm-poky-linux-gnueabi- -C . M=drivers/char/nrf24/
$ scp nrf24.ko root@<IP of PI>:/home/root/.
```

### Server Setup
```
$ cd server
$ . /opt/poky/3.2.2/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi
$ make lib/libwireless.a
$ make
$ make test
$ scp server root@<IP of PI>:/home/root/.
$ scp test root@<IP of PI>:/home/root/.
```

### Server Init
```
$ insmod nrf24.ko
$ ./test
$ ./server
```

---

## Remote Debug Server
Target
```
$ gdbserver :1234 server
```
Host
```
$ arm-poky-linux-gnueabi-gdb server
(gdb) target remote <IP of PI>:1234
```
---

## Release Commits (Test Longloop)
A test named Test Longloop must pass in all release commits.
|Releas Date|Driver Branch and Commit Hash<br>Firmware and Design Branch and Commit Hash|
|----|---|
|20210517|linux@nRF24L01driver:8a4c0cdf7803df328cc4730b5077f52fee5dc932<br>AVR-nRF24@master:3eb939d17d8ff22d65403e109d810f7f85f86c85|
|20210523|linux@nRF24L01driver:8a4c0cdf7803df328cc4730b5077f52fee5dc932<br>AVR-nRF24@master:7b25dc61fc849677e8bb6358456cee5dc74c7301|
|20210531|linux@nRF24L01driver:7c86b9ca6b2d7ad1549c76c9caeeb97bab567bfb<br>AVR-nRF24@master:dd0c3b836620fbdc131a8c8d285148fa6ca5f2e6|
|20210820|inux@nRF24L01driver:7c86b9ca6b2d7ad1549c76c9caeeb97bab567bfb<br>AVR-nRF24@master:85c61a1c3e08124cf29ecca4751b73a7c155e9e8|
|20210909|linux@nRF24L01driver:f5a8eb844164f856415a3b459346a1253c0f87ff<br>AVR-nRF24@master:ca4479ed1e2c967fedf4eaf3a40d907f1cda15d0|
|20210919|linux@nRF24L01driver:f5a8eb844164f856415a3b459346a1253c0f87ff<br>AVR-nRF24@master:d2804b9a1e1fc83868c6ceb204d5eec31b9cb42f|

