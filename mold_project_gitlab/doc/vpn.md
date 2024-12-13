# vpn

## Description

Every device connects to a common vpn service under the url `vpn.probeview.com`. The client/box gets configured by the file `/etc/openvpn/client.conf`

If a box is connected to the vpn, there will be an IP in the hidden "Debug Status Screen" on the mold-ui.

## Prerequisits windows
install openvpn https://openvpn.net/community-downloads/  
add OpenVPN/bin folder to path if you want to call it from the cmd line  
isntall git bash https://git-scm.com/downloads

## join the vpn

```bash
ssh vpn@vpn.probeview.com -p 20022
```
copy the file `docker_debug_client.ovpn` to your device by
```bash
scp -P 20022 vpn@vpn.probeview.com:/home/vpn/docker_debug_client.ovpn .
```

### join linux:
```bash
sudo openvpn docker_debug_client.ovpn
```

### join windows:
```bash
openvpn docker_debug_client.ovpn
```
or open the `docker_debug_client.ovpn` in the OpenVPN client and connect

## connect to a box over vpn

Find out the IP of the device (Debug Status Screen). Join the VPN. Connect via 
```bash
ssh mlanner@<box-ip> -p 20022
```

## configure openvpn server

the openvpn server is dockerized.
```bash
ssh vpn@vpn.probeview.com -p 20022
./run_vpn_settings_docker
```
you are now inside a container attached to the openvpn container. Inside this container you may check logs, generate additional keys or change configs.

## possible improvements

find out about support of the openvpn for dns resolution. It'd be awesome to connect to the box by id, instead of the IP.

## server management

vpn server is available through
```bash
ssh -p 20022 vpn@vpn.probeview.com
```
it handles the vpn. Attention: systemd handles the life of the container: `/etc/systemd/system/docker-open_vpn.service`

### docker vpn documentation

we are using a dockerized version of openvpn.
https://github.com/kylemanna/docker-openvpn


### cert generation

the following command will create and print certificate for a device (`<device_name>`)

```bash
ssh -p 20022 vpn@vpn.probeview.com -- ./generate_cert <device_name>
# to remove the log output from stderr.
ssh -p 20022 vpn@vpn.probeview.com -- ./generate_cert <device_name> 2>/dev/null
# result to file.
ssh -p 20022 vpn@vpn.probeview.com -- ./generate_cert <device_name> > success.ovpn
```

the script will print the content of a `*.ovpn` file to the terminal. Use a private/public key for no password.

### get list of connected clients

```bash
docker exec -it probe_vpn cat /tmp/openvpn-status.log
```

### revoke a certificate

```bash
docker run \
  -v ovpn-data:/etc/openvpn \
  --log-driver=none \
  --rm \
  -it \
  kylemanna/openvpn ovpn_revokeclient <device_name>
# follow instructions on screen
docker restart probe_vpn
```

### backup

backups the folder `openvpn` everything to the current directory:
```
docker cp probe_vpn:/etc/openvpn .
```
