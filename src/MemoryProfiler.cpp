#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/MemoryProfiler.h"
#include <iostream>
#include <iomanip>

using namespace std;

namespace MemoryProfiler {
    atomic<size_t> cell_constructor_calls{ 0 };
    atomic<size_t> cell_move_constructor_calls{ 0 };
    atomic<size_t> cell_copy_constructor_calls{ 0 };
    atomic<size_t> cell_copy_assignment_calls{ 0 };
    atomic<size_t> cell_move_assignment_calls{ 0 };

    void reset() {
        cell_constructor_calls = 0;
        cell_move_constructor_calls = 0;
        cell_copy_constructor_calls = 0;
        cell_copy_assignment_calls = 0;
        cell_move_assignment_calls = 0;
    }

    void print_report() {
        cout << "\n3. Memory usage\n";
        size_t total_cell_calls = cell_constructor_calls + cell_move_constructor_calls + cell_copy_constructor_calls;
        cout << "Total cell operations: " << total_cell_calls << "\n";
        cout << "Constructors: " << cell_constructor_calls << " (" << fixed << setprecision(1) << (100.0 * cell_constructor_calls / total_cell_calls) << "%)\n";
        cout << "Move constructors: " << cell_move_constructor_calls << " (" << fixed << setprecision(1) << (100.0 * cell_move_constructor_calls / total_cell_calls) << "%)\n";
        cout << "Copy constructors: " << cell_copy_constructor_calls << " (" << fixed << setprecision(1) << (100.0 * cell_copy_constructor_calls / total_cell_calls) << "%)\n";
        cout << "Copy assignments: " << cell_copy_assignment_calls << " (" << fixed << setprecision(1) << (100.0 * cell_copy_assignment_calls / total_cell_calls) << "%)\n";
        cout << "Move assignments: " << cell_move_assignment_calls << " (" << fixed << setprecision(1) << (100.0 * cell_move_assignment_calls / total_cell_calls) << "%)\n";
    }
}