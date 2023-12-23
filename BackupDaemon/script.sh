#!/bin/bash

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root or using sudo."
    exit
fi

# Set paths and names
DIR="/usr/local/bin/backupdaemon_service"
SERVICE_NAME="backupdaemon"

# Compile C++ code
g++ daemon.cpp -std=c++20 -O2 -o $SERVICE_NAME

# Create directory if not exists
mkdir -p $DIR

# Copy files to the directory
cp $SERVICE_NAME config.yaml $DIR
chmod +x $DIR/$SERVICE_NAME

# Create systemd service file
cat << EOF > /etc/systemd/system/${SERVICE_NAME}.service
[Unit]
Description=Backup Daemon Service
After=network.target

[Service]
Type=simple
Restart=no
User=root
WorkingDirectory=$DIR
ExecStart=$DIR/$SERVICE_NAME

[Install]
WantedBy=default.target
EOF

# Set permissions and reload systemd
chmod 664 /etc/systemd/system/${SERVICE_NAME}.service
systemctl daemon-reload

# Enable and start the service
systemctl enable ${SERVICE_NAME}.service
systemctl start ${SERVICE_NAME}.service

echo "Script finished"

