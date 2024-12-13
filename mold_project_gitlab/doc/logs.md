# testing gsm

## prerequists

- login
```bash
ssh mlanner@<ip-or-dns-of-rpi> -p 20022
```
- switch to root
```bash
sudo -i
```

## logs

### system log

`journalctl` collects, manages and displays all system logs. Eg. USB failure will get logged here. Run it to get ALL logs from all boots.
```bash
journalctl
```

to get the logs from the current boot
```bash
journalctl -b 0
```

list all boots
```bash
journalctl --list-boots
```

to list the last 10 lines of the system log and follow it as it grows, run
```bash
journalctl -f
```

### mold logs

the current mold log is in `/opt/mold/`. The older logs are in `/opt/mold/logs`. To list the current log run

```bash
cd /opt/mold
less mold_00000.log
```

To raise the verbosity edit the file `/opt/mold/gui.cfg` and change `log-severity` from `log-severity=error` to `log-severity=verbose`

### gsm logs

by default the `gsm_mux` program does not log, to rest the SD-Card. To enable logging you have to edit the file `/root/start` and change the line 

```bash
/opt/mold/exec-gsm_mux 2>&1 > /dev/null &`
```

to 

```bash
date
/opt/mold/exec-gsm_mux 2>&1 >> /root/gsm.log &`
```

after a reboot there will be a file `/root/gsm.log` with the gsm and rssi information.

The `>>` means that the program will APPEND its log to the file. use `>` to always write a new log file (and delete the old one)

## running the programs in console mode instead of daemon mode

Instead of running the programs in the background (daemon) and checking the log files. You can run the programs in foreground.
First you have to kill the background proccesses and then run them in the foreground.

### mold

kill the background proccesses
```bash
pkill -c mold
```

`-c` prints the count of prcocesses killed. It should print 2.

Enable console logging by editing the config file `/opt/mold/gui.cfg`. change the lines

```ini
log-flush=0
log-console=0
```

to 

```ini
log-flush=1
log-console=1
```

then switch to the mold directory, load the environment and start mold

```bash
cd /opt/mold
source enviroment
./bin/mold -c gui.cfg
```

### gsm_mux

kill the background proccesses

```bash
pkill -c gsm
```

`-c` prints the count of prcocesses killed. It should print 2.

then switch to the mold directory, load the environment and start gsm_mux

```bash
cd /opt/mold
source enviroment
./bin/gsm_mux
```

for options and help run

```bash
./bin/gsm_mux --help
```
