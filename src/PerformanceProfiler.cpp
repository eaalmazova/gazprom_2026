#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/PerformanceProfiler.h"

void PerformanceProfiler::start_section(const std::string& name) {
    auto& stats = get_instance().sections[name];
    stats.start_time = std::chrono::high_resolution_clock::now();
    stats.is_running = true;
}

void PerformanceProfiler::end_section(const std::string& name) {
    auto& stats = get_instance().sections[name];
    if (stats.is_running) {
        auto end_time = std::chrono::high_resolution_clock::now();
        stats.total_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - stats.start_time);
        stats.call_count++;
        stats.is_running = false;
    }
}

void PerformanceProfiler::print_report() {
    auto& instance = get_instance();
    std::cout << "\n4. Hotspots\n";
    std::cout << std::left << std::setw(30) << "Component" << std::setw(15) << "Time (ms)" << std::setw(15) << "Calls" << std::setw(15) << "Avg time (micros)" << "\n";
    std::chrono::nanoseconds total_time{ 0 };
    for (const auto& [name, stats] : instance.sections) {
        total_time += stats.total_time;
    }
    for (const auto& [name, stats] : instance.sections) {
        double ms = stats.total_time.count() / 1000000.0;
        double avg = stats.total_time.count() / (stats.call_count * 1000.0);
        std::cout << std::left << std::setw(30) << name << std::setw(15) << std::fixed << std::setprecision(2) << ms << std::setw(15) << stats.call_count << std::setw(15) << std::fixed << std::setprecision(1) << avg << std::endl;
    }
}

void PerformanceProfiler::reset() {
    get_instance().sections.clear();
}

PerformanceProfiler& PerformanceProfiler::get_instance() {
    static PerformanceProfiler instance;
    return instance;
}