#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <filesystem>
#include <cstdlib>
#include <csignal>
#include <syslog.h>
#include <chrono>
#include <thread>

#include "process.h"

constexpr unsigned CHECK_INTERVAL_MS = 2000;

std::vector<Process> getNewProcesses(const std::unordered_map<unsigned, Process> &old,
                                     const std::unordered_map<unsigned, Process> &now) {
    std::vector<Process> new_processes;
    for (auto &element: now) {
        if (!element.second.command.empty() &&
            (!old.contains(element.first) || old.at(element.first).uptime > element.second.uptime)) {
            new_processes.push_back(element.second);
        }
    }

    return new_processes;
}

std::vector<Process> getExpiredProcesses(const std::unordered_map<unsigned, Process> &old,
                                         const std::unordered_map<unsigned, Process> &now) {
    std::vector<Process> expired_processes;
    for (auto &element: old) {
        if (!now.contains(element.first) && element.second.uptime != 0) {
            expired_processes.push_back(element.second);
        }
    }

    return expired_processes;
}

inline std::string makeMessage(const Process &process, bool is_expired = false) {
    std::string result = (is_expired) ? "# Expired process:" : "# New process:";
    result += "\nName: " + process.name +
              "\nProcess ID: " + process.pid +
              "\nUser: " + process.user +
              "\nUptime: " + std::to_string(process.uptime) +
              "\nCommand: " + process.command +
              "\n---------------------------\n";
    return result;
}

std::string getProcessStatusValue(const std::string &pid, const std::string &key) {
    std::ifstream statusFile("/proc/" + pid + "/status");

    if (!statusFile.is_open()) {
        std::string err_message = "Error opening file: /proc/" + pid + "/status";
        std::cerr << err_message << std::endl;
        syslog(LOG_ERR, "%s", err_message.c_str());
        exit(1);
    }

    std::string line;

    while (std::getline(statusFile, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == key + ":") {
            std::string value;
            iss >> value;
            statusFile.close();
            return value;
        }
    }

    statusFile.close();
    return "";
}

unsigned getUptime(const std::string &pid) {
    std::ifstream statFile("/proc/" + pid + "/stat");

    if (!statFile.is_open()) {
        std::string err_message = "Error opening file: /proc/" + pid + "/stat";
        std::cerr << err_message << std::endl;
        syslog(LOG_ERR, "%s", err_message.c_str());
        exit(1);
    }

    std::string line, dummy;
    unsigned uptime;

    getline(statFile, line);
    statFile.close();

    std::istringstream iss(line);

    for (int i = 0; i < 13; ++i) {
        iss >> dummy;
    }
    iss >> uptime;

    return uptime;
}

std::unordered_map<unsigned, Process> getpids() {
    std::unordered_map<unsigned, Process> processes;

    DIR *dir;
    struct dirent *entry;

    dir = opendir("/proc");

    if (dir == nullptr) {
        std::string err_message = "Error opening directory: /proc";
        syslog(LOG_ERR, "%s", err_message.c_str());
        exit(1);
    }

    while ((entry = readdir(dir)) != nullptr) {
        if (isdigit(entry->d_name[0])) {
            std::string pid = entry->d_name;
            unsigned pid_int = std::stoi(pid);

            std::string user = getProcessStatusValue(pid, "Uid");
            std::string name = getProcessStatusValue(pid, "Name");

            std::string command;
            std::ifstream cmdlineFile("/proc/" + pid + "/cmdline");

            if (!cmdlineFile.is_open()) {
                std::string err_message = "Error opening file: /proc/" + pid + "/cmdline";
                std::cerr << err_message << std::endl;
                syslog(LOG_ERR, "%s", err_message.c_str());
                exit(1);
            }

            std::getline(cmdlineFile, command);

            unsigned uptime = getUptime(pid);

            processes.emplace(pid_int, Process{pid, user, name, command, uptime});
        }
    }

    closedir(dir);
    return processes;
}

void signalHandler(int signum) {
    syslog(LOG_NOTICE, "AuditDaemon stopped");
    closelog();
    exit(signum);
}

void openLog() {
    for (int signum = 0; signum < NSIG; ++signum) {
        signal(signum, signalHandler);
    }

    openlog("AuditDaemon", LOG_PID, LOG_DAEMON);

    syslog(LOG_NOTICE, "AuditDaemon started");
}

int main() {
    openLog();

    std::ofstream outputFile("results.txt", std::ios_base::app);

    std::unordered_map<unsigned, Process> processes = std::move(getpids());

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(CHECK_INTERVAL_MS));
        std::unordered_map<unsigned, Process> tmp = std::move(getpids());

        std::vector<Process> new_processes = std::move(getNewProcesses(processes, tmp));
        std::vector<Process> expired_processes = std::move(getExpiredProcesses(processes, tmp));

        for (const auto &newProcess: new_processes) {
            const std::string message = std::move(makeMessage(newProcess));
            outputFile << message;
            outputFile.flush();
            syslog(LOG_INFO, "%s", message.c_str());
        }

        for (const auto &expiredProcess: expired_processes) {
            const std::string message = std::move(makeMessage(expiredProcess, true));
            outputFile << message;
            outputFile.flush();
            syslog(LOG_INFO, "%s", message.c_str());
        }

        processes = std::move(tmp);
    }

    outputFile.close();
    return 0;
}
