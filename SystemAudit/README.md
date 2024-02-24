# Process Monitoring Application

This application monitors running processes in your system and logs information about them to a file and system log.

## Prerequisites

Ensure you have the following installed:

- C++20 compatible compiler (e.g., g++)
- Standard C++ libraries

## Compilation

1. Clone the repository:
```
https://github.com/finstape/system-programming-in-linux/tree/main/SystemAudit
```
2. Navigate to the project directory:
```
cd your_project
```
3. Compile the program using the following command:
```
g++ main.cpp -std=c++20 -o AuditDaemon
```
## Running

1. After successful compilation, execute the program by running:
```
./AuditDaemon
```
2. The program will start monitoring running processes and recording information about them to the "results.txt" file.

3. To view real-time updates of the recorded information, you can use the tail command with the -f flag:
```
tail -f results.txt
```