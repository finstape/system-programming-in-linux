# IP Address Checker App

This application allows you to perform scanning on specified IP addresses and ports to check for available services. It also provides information about the hosts such as ISP, country, region, city, and MAC address.

## Prerequisites

Make sure you have the following installed:

- Python 3.x
- `scapy`, `requests`, and `customtkinter` Python packages. You can install them using pip:
```
pip install scapy requests customtkinter
```
## Usage

1. Clone the repository:
```
https://github.com/finstape/system-programming-in-linux/tree/main/NetScanner
```
2. Navigate to the project directory:
```
cd your_project
```
3. Run the application using the following command:
```
sudo python3 main.py`
```
Note: Running with `sudo` may be necessary to perform certain network operations.

4. Enter the IP addresses and ports you want to scan in the respective textboxes. You can specify multiple IP addresses and ports, separated by new lines.
5. Optionally, you can enable the "Full scan" checkbox to scan all ports (1-65535).
6. Click the "Check" button to initiate the scanning process.
7. Results will be displayed in the text area, including statistics such as total hosts scanned and open ports found.
