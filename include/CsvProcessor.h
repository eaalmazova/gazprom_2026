#ifndef CSVPROCESSOR_H
#define CSVPROCESSOR_H

#include <string>
#include <memory>
#include <chrono>
#include <atomic>
#include <map>
#include <vector>
#include <fstream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <utility>
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/CsvGridParser.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/ThreadSafeQueue.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/ThreadPool.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/GridBlock.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/NeighborhoodAveragePolicy.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/MemoryProfiler.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/PerformanceProfiler.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/GridAggregator.h"

using namespace std;

class CsvProcessor {
public:
    CsvProcessor(const string& input_path,
        const string& output_path,
        size_t num_threads = thread::hardware_concurrency());
    bool process();

    struct Statistics {
        size_t total_blocks = 0;
        size_t total_cells = 0;
        size_t error_cells = 0;
        chrono::milliseconds processing_time{ 0 };
    };

    const Statistics& getStatistics() const { return m_stats; }

private:
    void producer_task();
    void consumer_task(unique_ptr<GridBlock> block, size_t block_index);
    void process_results();
    bool writeHeader();
    bool writeBlock(const GridBlock& block, const vector<vector<Cell>>& processed_rows);
    void print_profiling_report();

    string m_input_path;
    string m_output_path;
    size_t m_num_threads;
    ThreadSafeQueue<pair<size_t, pair<unique_ptr<GridBlock>, vector<vector<Cell>>>>> m_result_queue;

    unique_ptr<CsvGridParser> m_parser;
    ofstream m_output_file;

    ThreadPool m_thread_pool;
    GridAggregator<> m_aggregator;

    Statistics m_stats;
    chrono::time_point<chrono::high_resolution_clock> m_start_time;

    atomic<size_t> m_blocks_processed{ 0 };
    atomic<size_t> m_blocks_written{ 0 };
    mutex m_write_mutex;
};

#endif