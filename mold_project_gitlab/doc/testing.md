# Testing

## Prerequists

### Install rpi boot

```bash
sudo apt-get install -y libusb-1.0-0-dev
git clone --depth=1 https://github.com/raspberrypi/usbboot
cd usbboot
make
```
You now got an executable called `rpiboot` in the directory `usbboot`.

## Getting an image

Use a file browser and go to `sftp://85.125.80.52/ftp/HSBimages/`.
The username is `github`.
Copy the image you want to test onto your local work station. The copied image (at least 400MB) is the image you want to test.

## Prepare compute module

plug in the compute module into the outbreak board. call `sudo <path-to-usbboot>/rpiboot`. A new device should appear. Find it with the command `sudo fdisk -l`. Its name should be like `/dev/sdX`, where `X` is a single letter, eg `/dev/sdb`. It will be your target-device.

## Flash the compute module

The flashing needs around 15 minutes and is done by:
```bash
gunzip -c <path-to-image> | pv | sudo dd of=<target-device>
```
`gunzip` decompressed it. `pv` will render the progress (it can be left out) and `dd` will do the actual writing to the compute module.
