#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/CsvProcessor.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/SingleThreadProcessor.h"
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace std;

int main() {
    try {
        string input_path = "C:/Users/Almaz/source/repos/ConsoleApplication3/data/input.csv";
        string output_single = "C:/Users/Almaz/source/repos/ConsoleApplication3/data/output_single.csv";
        string output_multi = "C:/Users/Almaz/source/repos/ConsoleApplication3/data/output_multi.csv";
        cout << "1. Single-thread\n";
        SingleThreadProcessor single_processor(input_path, output_single);
        auto single_time = single_processor.process();
        cout << "Processing time: " << single_time.count() << " ms\n\n";
        cout << "2. Multi-thread\n";
        CsvProcessor multi_processor(input_path, output_multi, 8);
        auto multi_start = chrono::high_resolution_clock::now();
        bool multi_success = multi_processor.process();
        auto multi_end = chrono::high_resolution_clock::now();
        if (!multi_success) {
            cerr << "Multi-thread processing failed!\n";
            return 1;
        }
        auto multi_time = chrono::duration_cast<chrono::milliseconds>(multi_end - multi_start);
        cout << "Processing time: " << multi_time.count() << " ms\n\n";
        cout << "5. Performance comparison\n";
        double speedup = static_cast<double>(single_time.count()) / multi_time.count();
        cout << left << setw(25) << "Single-thread time:" << single_time.count() << " ms\n";
        cout << left << setw(25) << "Multi-thread time:" << multi_time.count() << " ms\n";
        cout << left << setw(25) << "Speedup:" << fixed << setprecision(2) << speedup << " times\n";
        return 0;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}