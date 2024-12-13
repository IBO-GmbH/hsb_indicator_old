# Information about how to create a new image

## Basic preparation
* Flash image
* Put `ssh` file on boot partition
* Boot up
* Connect 

```sh
ssh pi@raspberrypi
sudo -i
```

### Expand partition
```sh
raspi-config
```

1. Select "Update"
2. Select "Advanced options", then "Expand Filesystem"

### I2c aktivieren und Serielle Schnittstelle
raspi-config

3. Interface options
P5 Enable I2C
P6 Disable Shell and activate serial


## User, hostname, ssh port
### Change ssh port
```sh
cat /etc/ssh/sshd_config | sed 's/#Port 22/Port 20022/g' | tee /etc/ssh/sshd_config
```

### Create new user
```sh
adduser hsb
usermod -aG sudo hsb

### user hsb without entering sudo password
sudo nano /etc/sudoers.d/010_pi-nopasswd

add line:
hsb ALL=(ALL) NOPASSWD: ALL
```

### Login as the new user
```sh
exit
exit
ssh hsb@raspberrypi
sudo -i
```

### Delete the old user
```sh
deluser pi
```

### Change default hostname
Note: The actual mold script will change the hostname to the mac address, later. However, during setup,
we change the base hostname
```sh
echo mold>/etc/hostname

add the new hostname to /etc/hosts at the end of line, this prevents sudo from firing some error at logon

e.g. 127.0.0.1 localhost mold
```

### Reboot
```sh
reboot
```

## Image setup
### Connect again
```sh
ssh -p 20022 hsb@mold
sudo -i
```

### Update
```sh
apt update && apt upgrade -y 
update-ca-certificates
```

### Install various dependencies
```sh
apt install -y haveged && systemctl enable haveged
apt install -y wiringpi libpng12-0 evtest libts-bin i2c-tools symlinks (network-manager not necessary anymore)
apt install -y libgles-dev libc6-dev libpng-dev libts-dev libssl-dev libsystemd-dev libasan2
```

### Prepare display
```sh
cat <<EOF >>/boot/config.txt
dtoverlay=rpi-display,speed=32000000,rotate=90
dtoverlay=rpi-backlight
dtoverlay=i2c-rtc,pcf8523
dtoverlay=pi3-disable-bt
dtoverlay=sc16is750-i2c,xtal=1843200,int_pin=19,addr=0x68
dtparam=i2c_arm=on
EOF
```

### das ist nicht mehr notwendig bei Buster - Prepare i2c
```sh
echo i2c-dev>>/etc/modules
echo i2c-bcm2708>>/etc/modules
```

### Build `n_gsm`
```sh
apt install -y raspberrypi-kernel-headers gcc
mkdir ~/ngsm_build_dir
cd ~/ngsm_build_dir
wget https://raw.githubusercontent.com/torvalds/linux/v4.19/drivers/tty/n_gsm.c .

cat << "EOF" >Makefile
obj-m += n_gsm.o

all:
  make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
  make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
EOF

sed -i -e 's/  /\t/g' Makefile
make
mv -v n_gsm.ko /lib/modules/$(uname -r)/kernel/drivers/tty/
depmod
cd ~
```

### Distribute `n_gsm`
* You need to perform that build step for every platform (CM1, CM3, CM3+) and copy together all `n_gsm.ko` files 
* Execute `depmod` on every different CM/Kernel (so, at some point, you need to pull the image and write it to another platform and build again)


### Remove build dependencies
```sh
apt purge -y raspberrypi-kernel-headers
apt autoremove -y
```

### Speed up bootup
```sh
systemctl disable systemd-networkd-wait-online.service
rm /etc/systemd/system/dhcpcd.service.d/wait.conf
systemctl disable raspi-config.servicesu
systemctl disable dphys-swapfile.service

cat <<EOF >>/boot/config.txt
# Disable the rainbow splash screen
disable_splash=1
EOF

apt purge -y bluez
apt autoremove -y
```

### Prepare USB for Modem
```
sudo modprobe option
sudo echo "2c7c 0700" > /sys/bus/usb-serial/drivers/option1/new_id
```

Now edit rc.local, so modprobe option starts also after reboot
Insert before exit 0
```
sudo modprobe option
sudo echo "2c7c 0700" > /sys/bus/usb-serial/drivers/option1/new_id
```

reboot now and after reboot check if modem is available
```
gpio mode 16 out
gpio write 16 0
gpio write 16 1
gpio write 16 0
```

Check ob ttyUSB* da sind mit ls /dev/ttyU*

### Modemtreiber installieren
Modem Treiber mit PPP von Sixfab
Beschreibung hier: https://docs.sixfab.com/page/setting-up-the-ppp-connection-for-sixfab-shield-hat
```
wget https://raw.githubusercontent.com/sixfab/Sixfab_PPP_Installer/master/ppp_install_standalone.sh
sudo chmod +x ppp_install_standalone.sh
sudo ./ppp_install_standalone.sh
```
option 2 auswählen, im install script  

APN = m2m.business  
Kein user/passwort  

Schnittstelle ttyUSB2  

no autoconnect, you can connect with sudo pon, disconnect with sudo poff


### Install wifi driver

```sh
**********************************************************
sudo wget http://downloads.fars-robotics.net/wifi-drivers/install-wifi -O /usr/bin/install-wifi
sudo chmod +x /usr/bin/install-wifi
install-wifi
reboot
*********************************************************

Network Manager Infos: http://www.intellamech.com/RaspberryPi-projects/rpi_nmcli.html

------------------------------------------------------
### ???? Uninstall dhcpd
```sh
apt purge -y openresolv dhcpcd5

??? No network connection after this step ???
```
---------------------------------------------------
### Prepare hardware clock
```sh
apt remove -y fake-hwclock
update-rc.d -f fake-hwclock remove
systemctl disable fake-hwclock
sed -i 's/\/run\/systemd\/system/\/DISABLED\/run\/systemd\/system/g; s/\/sbin\/hwclock --rtc=$dev --systz/#\/sbin\/hwclock --rtc=$dev --systz/g' /lib/udev/hwclock-set
```

### Touchscreen
```sh
ln -s /usr/lib/arm-linux-gnueabihf/libts.so.0 /usr/lib/arm-linux-gnueabihf/libts-0.0.so.0
echo "150 -5990 22007998 4263 -11 -847646 65536">/etc/pointercal
```

### Convert symlinks
```sh
symlinks -rc /
```

### Disable tty service on serial ttyAMA0
```sh
systemctl stop serial-getty@ttyAMA0.service
systemctl disable serial-getty@ttyAMA0.service
```

### Create startup script templates
```sh
cat << EOF >>/root/prepare
#!/bin/bash

echo "Empty image prepare script. Simulation preparation"
sleep 3
echo "Done with simulated preparation"
EOF

cat << EOF >>/root/run
#!/bin/bash

echo "Service script template"
sleep 1h
EOF

chmod u+x /root/prepare
chmod u+x /root/run

cat << EOF >/etc/systemd/system/mold-prepare.service
[Unit]
Description=Mold preparation service

[Service]
Type=oneshot
ExecStart=/root/prepare
RemainAfterExit=true

[Install]
EOF

cat << EOF >/etc/systemd/system/mold.service
[Unit]
Description=Mold service
Wants=mold-prepare.service
After=mold-prepare.service

[Service]
Type=simple
ExecStart=/root/run
Restart=always
RestartSec=3

[Install]
WantedBy=multi-user.target
EOF

chmod 644 /etc/systemd/system/mold-prepare.service
chmod 644 /etc/systemd/system/mold.service

systemctl enable mold
```

# Use new image
## Get image from raspberry
* Launch balena etcher as `sudo`
* Attach programmer and wait for sd card to be mounted
```
daniel@dmlinuxdev:~/Downloads $ df -h
Filesystem      Size  Used Avail Use% Mounted on
...
/dev/sda1       253M   53M  200M  21% /media/daniel/boot
/dev/sda2       3,4G  1,4G  1,8G  45% /media/daniel/rootfs
```
* Fill image with zeros: 
```sh
sudo dd if=/dev/zero of=/media/daniel/rootfs/zero
sudo rm /media/daniel/rootfs/zero
```
* Copy over from raspberry.
```sh
sudo dd if=/dev/sda of=image_buster_7.img status=progress
```

## Compress image
```sh
sudo apt install -y pixz
pixz -k image_buster_7.img image_buster_7.img.pxz
```

## Upload dependency
```sh
scp -P 20022 ./image_buster_7.img.pxz root@cirunner.linkbit.io:/home/mlanner/mold/http_server_files/
```
find pwd here https://gitlab.linkbit.io:20443/colyoo/colyoo_documentation/-/wikis/Server-hosts-and-ports

## Update source
* Update image name [here](https://gitlab.linkbit.io:20443/mold/mold/blob/18d66eaa04ecd079b2a70cf69a87810cdeb27883/build/dependencies/get.d#L49)
* Get offset of second partition:
```
daniel@dmlinuxdev:~/Downloads $ fdisk -l image_buster_7.img
Disk image_buster_7.img: 3,7 GiB, 3909090816 bytes, 7634943 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x738a4d67

Device              Boot  Start     End Sectors  Size Id Type
image_buster_7.img1        8192  532479  524288  256M  c W95 FAT32 (LBA)
image_buster_7.img2      532480 7634943 7102464  3,4G 83 Linux
```
Note the start of the second partition and put it [here](https://gitlab.linkbit.io:20443/mold/mold/blob/5004d6bc238914b7d2dd552a074e3409d6eca5e1/build/make_build.d#L269), [here](https://gitlab.linkbit.io:20443/mold/mold/blob/9b302dcecae9731055f20422e9b376474bec16dd/build/prepare_base_image.d#L5), [here](https://gitlab.linkbit.io:20443/mold/mold/blob/74ffc77ae4a7b65390dd474d6992808b9ff26d3f/ci/tests/run_qemu#L7), [here](https://gitlab.linkbit.io:20443/mold/mold/blob/fb03fb22c7fd381e11537784f2aa7e29825a7bef/ci/exec_create_image#L39) and [here](https://gitlab.linkbit.io:20443/mold/mold/blob/fb03fb22c7fd381e11537784f2aa7e29825a7bef/ci/exec_create_image#L18)


*If that offset changes, I strongly suggest searching for the old "start" value (currently `532480`) and the already multiplied value (currently `272629760`) in the whole repository!*