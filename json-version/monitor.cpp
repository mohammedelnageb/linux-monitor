#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <sstream>
#include <string>
#include <mutex>
#include <atomic>
#include <csignal>
#include <ctime>
#include <sys/statvfs.h>
#include "json.hpp"

using json = nlohmann::json;

/* ===================== Global State ===================== */
std::mutex mtx;
std::atomic<bool> running(true);

/* ===================== Signal Handling ===================== */
void handleSignal(int) {
    running = false;
}

/* ===================== Utilities ===================== */
std::string getTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm tm{};
    localtime_r(&now, &tm);

    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buf);
}

void logToJson(const std::string& category, const std::string& message) {
    json j;
    j["timestamp"] = getTimestamp();
    j["category"]  = category;
    j["message"]   = message;

    std::ofstream file("/logs/system_log.json", std::ios::app);
    if (!file.is_open()) {
        return; // fail silently in production
    }
    file << j.dump() << std::endl;
}

/* ===================== Abstract Monitor ===================== */
class Monitor {
public:
    virtual void collect() = 0;
    virtual ~Monitor() = default;
};

/* ===================== CPU Monitor ===================== */
struct CpuData {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
};

class CpuMonitor : public Monitor {
    std::vector<CpuData> readCpu() {
        std::ifstream file("/proc/stat");
        std::vector<CpuData> data;
        std::string line;

        while (std::getline(file, line)) {
            if (line.rfind("cpu", 0) != 0) break;

            std::istringstream iss(line);
            std::string cpu;
            CpuData d{};
            iss >> cpu >> d.user >> d.nice >> d.system >> d.idle
                >> d.iowait >> d.irq >> d.softirq >> d.steal;
            data.push_back(d);
        }
        return data;
    }

    double usage(const CpuData& a, const CpuData& b) {
        auto idleA = a.idle + a.iowait;
        auto idleB = b.idle + b.iowait;

        auto totalA = a.user + a.nice + a.system + a.idle +
                      a.iowait + a.irq + a.softirq + a.steal;
        auto totalB = b.user + b.nice + b.system + b.idle +
                      b.iowait + b.irq + b.softirq + b.steal;

        auto totalDiff = totalB - totalA;
        auto idleDiff  = idleB - idleA;

        if (totalDiff == 0) return 0.0;
        return (totalDiff - idleDiff) * 100.0 / totalDiff;
    }

public:
    void collect() override {
        auto s1 = readCpu();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto s2 = readCpu();

        std::ostringstream out;
        for (size_t i = 0; i < s1.size(); ++i) {
            double u = usage(s1[i], s2[i]);
            if (i == 0)
                out << "Total CPU Usage: " << u << "%\n";
            else
                out << "CPU" << i - 1 << " Usage: " << u << "%\n";
        }

        std::lock_guard<std::mutex> lock(mtx);
        std::cout << out.str();
        logToJson("CPU", out.str());
    }
};

/* ===================== Memory Monitor ===================== */
struct MemData {
    unsigned long long total, free, buffers, cached;
};

class MemoryMonitor : public Monitor {
    MemData readMem() {
        std::ifstream file("/proc/meminfo");
        std::string line;
        MemData m{};

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            iss >> key;
            if (key == "MemTotal:")  iss >> m.total;
            if (key == "MemFree:")   iss >> m.free;
            if (key == "Buffers:")   iss >> m.buffers;
            if (key == "Cached:")    iss >> m.cached;
        }
        return m;
    }

public:
    void collect() override {
        MemData m = readMem();
        auto used = m.total - m.free - m.buffers - m.cached;
        double percent = (used * 100.0) / m.total;

        std::ostringstream out;
        out << "Memory Usage: " << percent << "%\n";

        std::lock_guard<std::mutex> lock(mtx);
        std::cout << out.str();
        logToJson("Memory", out.str());
    }
};

/* ===================== Disk Monitor ===================== */
class DiskMonitor : public Monitor {
public:
    void collect() override {
        struct statvfs stat;
        if (statvfs("/", &stat) != 0) return;

        auto total = stat.f_blocks * stat.f_frsize;
        auto free  = stat.f_bfree  * stat.f_frsize;
        auto used  = total - free;
        double percent = (used * 100.0) / total;

        std::ostringstream out;
        out << "Disk Usage (/): " << percent << "%\n";

        std::lock_guard<std::mutex> lock(mtx);
        std::cout << out.str();
        logToJson("Disk", out.str());
    }
};

/* ===================== Main ===================== */
int main() {
    signal(SIGINT,  handleSignal);
    signal(SIGTERM, handleSignal);

    CpuMonitor    cpu;
    MemoryMonitor mem;
    DiskMonitor   disk;

    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Starting Linux System Monitor...\n";
    }

    while (running) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "[INFO] Collecting system metrics...\n";
        }

        cpu.collect();
        mem.collect();
        disk.collect();

        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "----------------------------------------\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    std::cout << "Exiting Linux System Monitor...\n";
    return 0;
}

