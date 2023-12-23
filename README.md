# system-programming-in-linux
Materials for the practicum for "System programming in Linux" course at SpbU

# BackupDaemon

BackupDaemon is a C++ daemon for automated and regular backup of data from one directory to another.

## Overview

BackupDaemon is a background process that creates regular backups from a source directory to a backup directory. It integrates with systemd for easy management and monitoring.

## Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/finstape/system-programming-in-linux
    ```

2. Run the installation script:

    ```
    sudo bash script.sh
    ```

## Usage

BackupDaemon runs automatically, creating backups based on the configuration in `config.yaml`.

## Configuration
Edit config.yaml to specify source and backup directories, backup frequency, and other parameters.

```
sourceDirectory: /path/to/source
backupDirectory: /path/to/backup
frequency: minutely/hourly/daily/monthly
```

## Systemd Service Commands

```bash
systemctl status backupdaemon
systemctl [start/stop] backupdaemon
systemctl [enable/disable] backupdaemon
systemctl reload backupdaemon
```
# System Audit

Compile program with `g++ main.cpp` and then start program with `./a.out PID`
