# HSB

Projet for Human Smart Box

## Software Update via Swagger Interface
https://cloud.probeview.com:4432/docs/index.html#/

## building

### prerequists

Ubuntu 18.10 or newer is needed. `GLIBC_2.28` is needed.

### install dependencies

- install utils
```bash
sudo apt install -y ccache curl cmake git ninja-build pigz pixz wget xz-utils libffi-dev
```
- install D
```bash
curl -Lo dmd.deb http://downloads.dlang.org/releases/2.x/2.083.0/dmd_2.083.0-0_amd64.deb
sudo dpkg -i dmd.deb
rm -v dmd.deb 
```

### get dependencies, mount, configure and build

- get dependencies, mount, configure and generate cmake build
```bash
./build/make_build
```
- build with qt creator or `./build/install`

## Deploy

# connect to a device via ssh

```bash
ssh mlanner@<device_id>.local -p 20022
```

become root

```bash
sudo -i
```

write a bash script with the following content

```bash
#!/bin/bash

mkdir -p /opt/mold_dev
umount /opt/mold_dev
sshfs -o cache=no \
  -o no_readahead \
  -o sshfs_sync \
  -p <ssh-port-of-your-machine> \
  /opt/mold_dev \
  <user-of-your-machine>@<machine-dns>.local:<dir-to-mold>/mold/install
```

execute the mount script

```bash
chmod u+x ./<bash-script>
./<bash-script>
```

you should now have a working development deploy. run eg tests with

```bash
cd /opt/mold_dev
source enviroment
./bin/tests_gtest
```

## Contributing

Codestyle is 'Google' and the tool for formatting is `clang-format-7`

## Credits

TODO: Write credits

## License

All Rights Reserved

## VScode

Install MS C++ Extensions
Install CMake Tools 1.1.2 (or newer)

Add the following `.vscode/settings.json`:
```json
{
    "cmake.buildDirectory": "${workspaceRoot}/../build",
    "cmake.configureOnOpen": false,
    "cmake.generator": "Ninja",
    "cmake.loggingLevel": "trace",
    "C_Cpp.default.configurationProvider": "vector-of-bool.cmake-tools",
    "cmake.useCMakeServer": true
}
```

Add the following `.vscode/cmake-kits.json`:
```json
[
    {
        "name": "Colyoo Toolchain File",
        "toolchainFile": "${workspaceRoot}/../dependencies/toolchain_rpi.cmake"
    }
]
```

Restart cmake server after configuring the first time!

