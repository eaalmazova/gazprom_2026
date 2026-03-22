#ifndef MEMORYPROFILER_H
#define MEMORYPROFILER_H

#include <atomic>
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

namespace MemoryProfiler {
    extern atomic<size_t> cell_constructor_calls;
    extern atomic<size_t> cell_move_constructor_calls;
    extern atomic<size_t> cell_copy_constructor_calls;
    extern atomic<size_t> cell_copy_assignment_calls;
    extern atomic<size_t> cell_move_assignment_calls;
    
    void reset();
    void print_report();
}

#endif