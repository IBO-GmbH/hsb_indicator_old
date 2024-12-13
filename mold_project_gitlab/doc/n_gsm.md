# build deps
fetch the build dependencies  
```bash
apt-get install -y raspberrypi-kernel-headers gcc
```

# build directory
create a build directory
```bash
mkdir ngsm_build_dir
```

# n_gsm.c
copy one of the following n_gsm.c files into the build directory. you can use wget to download the file
```bash
wget https://raw.githubusercontent.com/torvalds/linux/v4.19/drivers/tty/n_gsm.c .
```

### New version for Buster:
https://raw.githubusercontent.com/torvalds/linux/v4.19/drivers/tty/n_gsm.c

### Version for Jessie
https://elixir.free-electrons.com/linux/v4.9.35/source/drivers/tty/n_gsm.c#

# Makefile
create a Makefile and copy the following content
```make
obj-m += n_gsm.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

# build
```bash
make
```

# install
```bash
mv -v n_gsm.ko /lib/modules/$(uname -r)/kernel/drivers/tty/
depmod
```

# Different versions
You need possibly build different versions for different raspberries. For example, on Buster, CM loads the kernel 4.19.75+ on and CM3 loads 4.19.75-v7+. You need to build for both and puth the file into both directories for an universal image.

**Important**: You need to execute `depmod` for each different CM/kernel.