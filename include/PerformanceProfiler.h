#ifndef PERFORMANCEPROFILER_H
#define PERFORMANCEPROFILER_H

#include <chrono>
#include <string>
#include <map>
#include <atomic>
#include <iostream>
#include <iomanip>

class PerformanceProfiler {
public:
    struct SectionStats {
        std::chrono::nanoseconds total_time{ 0 };
        size_t call_count = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
        bool is_running = false;
    };

    static void start_section(const std::string& name);
    static void end_section(const std::string& name);
    static void print_report();
    static void reset();

private:
    static PerformanceProfiler& get_instance();
    std::map<std::string, SectionStats> sections;
};

#endif