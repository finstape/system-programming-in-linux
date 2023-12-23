#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <syslog.h>
#include <thread>
#include <csignal>

void signalHandler(int signum) {
    syslog(LOG_NOTICE, "BackupDaemon stopped");
    closelog();
    exit(signum);
}


void readConfig(std::string& sourceDirectory, std::string& backupDirectory, unsigned& frequency) {
    std::ifstream config("config.yaml");

    if (!config.is_open()) {
        syslog(LOG_ERR, "ERROR: Cannot open file config.yaml");
    }

    std::string str;
    while (getline(config, str)) {
        std::istringstream line(str);
        std::string key, val;

        getline(line, key, ':');
        getline(line, val);
        val = val.substr(1);
        std::cerr << key << "-" << val << std::endl;

        if (key == "sourceDirectory") sourceDirectory = val;
        if (key == "backupDirectory") backupDirectory = val;
        if (key == "frequency") {
            if (val == "minutely") frequency = 1;
            else if (val == "hourly") frequency = 60;
            else if (val == "daily") frequency = 1440;
            else if (val == "weekly") frequency = 10080;
            else if (val == "monthly") frequency = 43200;
            else {
                syslog(LOG_ERR, "ERORR: Specified frequency format is not supported");
                signalHandler(1);
            }
        }
    }
}


std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S %d-%m-%Y");
    return ss.str();
}


std::string getDirectoryName(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.filename();
}


void backupDaemon(const std::string& sourceDirectory, const std::string& backupDirectory, unsigned &frequency) {
    while (true) {
        std::string backupFolder = backupDirectory + "/" + getDirectoryName(sourceDirectory) + "_backup " + getCurrentTime();

        if (!std::filesystem::create_directory(backupFolder)) {
            syslog(LOG_ERR, "ERROR: Cannot create backup directory");
            exit(1);
        }

        std::filesystem::copy(sourceDirectory, backupFolder, std::filesystem::copy_options::recursive);

        syslog(LOG_INFO, "Backup created successfully: %s", backupFolder.c_str());

        std::this_thread::sleep_for(std::chrono::minutes(frequency));
    }
}


int main() {
    signal(SIGTERM, signalHandler);

    openlog("BackupDaemon", LOG_PID, LOG_DAEMON);
    syslog(LOG_NOTICE, "BackupDaemon started");

    std::string sourceDirectory, backupDirectory;
    unsigned frequency;

    readConfig(sourceDirectory, backupDirectory, frequency);

    backupDaemon(sourceDirectory, backupDirectory, frequency);

    return 0;
}
