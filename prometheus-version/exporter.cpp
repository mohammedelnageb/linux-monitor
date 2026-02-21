#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/statvfs.h>

std::string getCPUUsage() {
    static long long lastIdle = 0, lastTotal = 0;
    std::ifstream file("/proc/stat");
    std::string cpu;
    long long user, nice, system, idle, iowait, irq, softirq, steal;

    file >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    long long idleTime = idle + iowait;
    long long totalTime = user + nice + system + idle + iowait + irq + softirq + steal;

    long long totalDiff = totalTime - lastTotal;
    long long idleDiff = idleTime - lastIdle;

    lastTotal = totalTime;
    lastIdle = idleTime;

    double usage = totalDiff ? (100.0 * (totalDiff - idleDiff) / totalDiff) : 0.0;

    std::ostringstream out;
    out << "cpu_usage_percent " << usage << "\n";
    return out.str();
}

std::string getMemoryUsage() {
    std::ifstream file("/proc/meminfo");
    std::string key;
    long long total = 0, free = 0, buffers = 0, cached = 0;

    while (file >> key) {
        if (key == "MemTotal:") file >> total;
        else if (key == "MemFree:") file >> free;
        else if (key == "Buffers:") file >> buffers;
        else if (key == "Cached:") file >> cached;
        else file.ignore(256, '\n');
    }

    long long used = total - free - buffers - cached;
    double percent = (used * 100.0) / total;

    std::ostringstream out;
    out << "memory_usage_percent " << percent << "\n";
    return out.str();
}

std::string getDiskUsage() {
    struct statvfs stat;
    statvfs("/", &stat);

    long long total = stat.f_blocks * stat.f_frsize;
    long long free = stat.f_bfree * stat.f_frsize;
    long long used = total - free;

    double percent = (used * 100.0) / total;

    std::ostringstream out;
    out << "disk_usage_percent{mount=\"/\"} " << percent << "\n";
    return out.str();
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9100);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    std::cout << "Exporter running on port 9100...\n";

    while (true) {
        int client = accept(server_fd, nullptr, nullptr);

        std::string metrics =
            "# HELP cpu_usage_percent CPU usage\n"
            "# TYPE cpu_usage_percent gauge\n" +
            getCPUUsage() +
            "# HELP memory_usage_percent Memory usage\n"
            "# TYPE memory_usage_percent gauge\n" +
            getMemoryUsage() +
            "# HELP disk_usage_percent Disk usage\n"
            "# TYPE disk_usage_percent gauge\n" +
            getDiskUsage();

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(metrics.size()) + "\r\n\r\n" +
            metrics;

        send(client, response.c_str(), response.size(), 0);
        close(client);

        sleep(1);
    }
}
