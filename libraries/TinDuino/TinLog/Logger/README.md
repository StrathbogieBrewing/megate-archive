# Logger

Create SD card with Raspberry Pi image

sudo dd if=2021-03-04-raspios-buster-armhf-lite.img of=/dev/mmcblk0 bs=4M conv=fsync

Add the following files to the boot partition

touch ssh

nano wpa_supplicant.conf

'''
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=<au>

network={
 ssid="Hermitage"
 psk="Hermitage"
}
'''

Monitor serial port with

sudo socat /dev/ttyUSB0,b9600 -
