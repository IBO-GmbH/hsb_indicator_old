# General
1. We do all logging to the system journal. Specifically, we utilize `systemd-cat` for logging. We can pass a priority using the `-p` option as well as an application identifier using the `-t` option.
2. Journal logging is configured to log only to the ramdisk (`/run`)
3. We use a helper script `logging/to_system_log` which redirects current logging from any program or tool to `systemd-cat`, parsing for priority string and filling out priority as needed
4. Several background jobs are responsible for filtering out specific messages and uploading them. They are executed hourly. The main script for that is `logging/hourly_jobs`

# How to examine logs when on the box
To see last log entries
```
journalctl -n 500
```

To see last and live logs
```
journalctl -f
```

To see last and live warnings
```
journalctl -f -p 5
```

# Log directories and helper files
Log directories are as follows:

| Directory | Description
| --------- | -----------
| `/run/log/mold` | Temporary directory used during log transformation
| `/var/log/mold` | Persistent log files are in here. Size-limited.
| `/var/log/mold_upload` | Compressed logs are here, waiting to be uploaded. Size-limited

The helper scripts are placed inside `/opt/mold/scripts/logging`. These are their purposes:

| Script | Description
| ------ | -----------
| `cleanup_persistent_dirs` | Cleans up the persistent log directory and the upload directory (enforces size limit)
| `delete_oldest_files` | Deletes the oldest files inside a directory. Called by `cleanup_persistent_dirs`
| `get_logs_for_upload` | Copies over log files from persistent log directory to the upload directory, compressing them
| `hourly_jobs` | Called by crontab from `/etc/cron.hourly/mold_logging`. Initiates all regular background tasks.
| `log_environment` | Reads/defaults all configuration environment variables for logging
| `logger` | Contains helper functions for logging, making it easier to pipe logging. See `/root/start` for usage.
| `save_crash_logs` | Called after an executable has ended. Saves last log entries from journal to persistent directory.
| `save_high_priority_logs` | Saves all log warnings, errors, criticals from the last hour to the persistent directory
| `to_system_log` | Used to pipe stdout/stderr to the journal. See `logger` for usage.
| `upload_log` | Uploads a single logfile to the logging web service. Deletes the file if uploaded or rate limit exceeded
| `upload_logs` | Calls `upload_log` on every file of the upload directory

# Rate and size limiting
We limit:
1. The number of persistent log bytes stored on the disk.
2. The number of compressed log bytes waiting to be uploaded
3. The number of bytes uploaded in a given amount of time. This limit is enforced by the server, by rejecting uploads after that limit has been exceeded.

*Note to point 3: The client sends an `Expect: 100-continue` header in all cases. If the rate limit is exceeded, the server will reject the request. The worst-behaviour case for a rate-limited client is that it sends HTTP headers every hour* 

# Configuration 
The most important configuration variables are:

| Variable | Default value | Description
| -------- | ------------- | -----------
| `LOG_PERSISTENT_DIR` | `/var/log/mold` | Directory where logs persist a reboot
| `LOG_UPLOAD_DIR` | `/var/log/mold_upload` | Directory for logs queued to be uploaded
| `LOG_PERSISTENT_MAX_BYTES` | `50000000` (50MB) | Maximum number of bytes in persistent log directory
| `LOG_UPLOAD_MAX_BYTES` | `50000000` (50MB) | Maximum number of bytes in upload directory
| `LOG_CRASH_NUMBER_OF_RECORDS` | `100` | Number of log records which are saved if an application crashes (`save_crash_logs`)

See `log_environment` for more configuration variables