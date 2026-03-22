#ifndef CELL_H
#define CELL_H

#include <iostream>
#include <optional>
#include <string>
#include <cmath>
#include <limits>
#include <atomic>
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/MemoryProfiler.h"

using namespace std;

struct Cell {
    double thermocurrent = 0.0;
    double thermopower = 0.0;
    double resistivity = 0.0;
    double temperature = 0.0;

    Cell() {
        MemoryProfiler::cell_constructor_calls++;
    }
    Cell(double tc, double tp, double res, double temp) : thermocurrent(tc), thermopower(tp), resistivity(res), temperature(temp) {
        MemoryProfiler::cell_constructor_calls++;
    }
    Cell(const Cell& other) : thermocurrent(other.thermocurrent), thermopower(other.thermopower), resistivity(other.resistivity), temperature(other.temperature) {
        MemoryProfiler::cell_copy_constructor_calls++;
    }
    Cell(Cell&& other) noexcept : thermocurrent(move(other.thermocurrent)), thermopower(move(other.thermopower)), resistivity(move(other.resistivity)), temperature(move(other.temperature)) {
        MemoryProfiler::cell_move_constructor_calls++;
    }
    
    Cell& operator=(const Cell& other) {
        if (this != &other) {
            thermocurrent = other.thermocurrent;
            thermopower = other.thermopower;
            resistivity = other.resistivity;
            temperature = other.temperature;
            MemoryProfiler::cell_copy_assignment_calls++;
        }
        return *this;
    }
    
    Cell& operator=(Cell&& other) noexcept {
        if (this != &other) {
            thermocurrent = move(other.thermocurrent);
            thermopower = move(other.thermopower);
            resistivity = move(other.resistivity);
            temperature = move(other.temperature);
            MemoryProfiler::cell_move_assignment_calls++;
        }
        return *this;
    }
    
    ~Cell() = default;

    static optional<double> parseDouble(const string& str) {
        if (str.empty()) {
            return nullopt;
        }
        if (str == "inf" || str == "+inf" || str == "INF" ||
            str == "Inf" || str == "Infinity") {
            return numeric_limits<double>::infinity();
        }
        if (str == "-inf" || str == "-INF" || str == "-Infinity") {
            return -numeric_limits<double>::infinity();
        }
        if (str == "nan" || str == "NaN" || str == "NAN") {
            return numeric_limits<double>::quiet_NaN();
        }

        try {
            size_t pos = 0;
            double value = stod(str, &pos);
            if (pos != str.length()) {
                return nullopt;
            }
            return value;
        }
        catch (const exception&) {
            return nullopt;
        }
    }
};

#endif