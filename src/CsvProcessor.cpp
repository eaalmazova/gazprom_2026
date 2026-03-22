#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/CsvProcessor.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/PerformanceProfiler.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <queue>
#include <condition_variable>

using namespace std;

CsvProcessor::CsvProcessor(const string& input_path, const string& output_path, size_t num_threads) : m_input_path(input_path), m_output_path(output_path), m_num_threads(num_threads), m_thread_pool(num_threads) {
    cout << "Initializing CsvProcessor with " << num_threads << " threads" << endl;
}

bool CsvProcessor::process() {
    MemoryProfiler::reset();
    PerformanceProfiler::reset();
    try {
        PerformanceProfiler::start_section("Total Processing");
        m_start_time = chrono::high_resolution_clock::now();
        PerformanceProfiler::start_section("File Open");
        m_output_file.open(m_output_path);
        PerformanceProfiler::end_section("File Open");
        if (!m_output_file.is_open()) {
            cerr << "Error: Cannot open output file: " << m_output_path << endl;
            return false;
        }
        PerformanceProfiler::start_section("Parser Initialization");
        m_parser = make_unique<CsvGridParser>(m_input_path);
        PerformanceProfiler::end_section("Parser Initialization");
        if (!writeHeader()) {
            return false;
        }
        thread producer_thread(&CsvProcessor::producer_task, this);
        process_results();
        producer_thread.join();
        m_thread_pool.wait_all();
        if (m_output_file.is_open()) {
            m_output_file.close();
        }
        PerformanceProfiler::end_section("Total Processing");
        print_profiling_report();
        return true;

    }
    catch (const exception& e) {
        cerr << "Error during processing: " << e.what() << endl;
        return false;
    }
}

void CsvProcessor::producer_task() {
    PerformanceProfiler::start_section("Producer Total");
    try {
        size_t block_index = 0;
        while (true) {
            PerformanceProfiler::start_section("Parser Read Block");
            auto block = m_parser->readNextBlock();
            PerformanceProfiler::end_section("Parser Read Block");

            if (!block) {
                break;
            }
            m_stats.total_blocks++;
            m_stats.total_cells += block->rows.size() * block->getColumnCount();
            size_t current_block_index = block_index;
            PerformanceProfiler::start_section("ThreadPool Submit");
            m_thread_pool.submit([this, block = move(block), current_block_index]() mutable {
                    this->consumer_task(move(block), current_block_index);
                }
            );
            PerformanceProfiler::end_section("ThreadPool Submit");
            block_index++;
        }

    }
    catch (const exception& e) {
        cerr << "Producer error: " << e.what() << endl;
    }
    PerformanceProfiler::end_section("Producer Total");
}

void CsvProcessor::consumer_task(unique_ptr<GridBlock> block, size_t block_index) {
    PerformanceProfiler::start_section("Consumer Total");
    try {
        vector<vector<Cell>> processed_rows;
        PerformanceProfiler::start_section("Aggregator Process");
        processed_rows = m_aggregator.processBlock(*block);
        PerformanceProfiler::end_section("Aggregator Process");

        PerformanceProfiler::start_section("Queue Push");
        {
            lock_guard<mutex> lock(m_write_mutex);
            m_result_queue.push(make_pair(block_index,
                make_pair(move(block), move(processed_rows))));
        }
        PerformanceProfiler::end_section("Queue Push");
        m_blocks_processed++;
    }
    catch (const exception& e) {
        cerr << "Consumer error in block " << block_index << ": " << e.what() << endl;
    }
    PerformanceProfiler::end_section("Consumer Total");
}

void CsvProcessor::process_results() {
    PerformanceProfiler::start_section("Results Processing");
    map<size_t, pair<unique_ptr<GridBlock>, vector<vector<Cell>>>> result_buffer;
    size_t next_block_to_write = 0;
    while (true) {
        pair<size_t, pair<unique_ptr<GridBlock>, vector<vector<Cell>>>> result;
        bool has_result = m_result_queue.try_pop(result);
        if (!has_result) {
            if (m_blocks_processed.load() > 0 &&
                m_blocks_processed == m_blocks_written &&
                m_parser->eof() &&
                m_thread_pool.get_task_count() == 0) {
                break;
            }
            this_thread::sleep_for(chrono::milliseconds(1));
            continue;
        }

        size_t block_index = result.first;
        auto& block_data = result.second;
        result_buffer[block_index] = move(block_data);
        while (result_buffer.find(next_block_to_write) != result_buffer.end()) {
            auto& data = result_buffer[next_block_to_write];
            PerformanceProfiler::start_section("Write Block");
            if (!writeBlock(*data.first, data.second)) {
                cerr << "Failed to write block " << next_block_to_write << endl;
            }
            PerformanceProfiler::end_section("Write Block");
            result_buffer.erase(next_block_to_write);
            next_block_to_write++;
            m_blocks_written++;
        }
    }
    while (!result_buffer.empty()) {
        auto it = result_buffer.begin();
        if (it != result_buffer.end()) {
            auto& data = it->second;
            if (!writeBlock(*data.first, data.second)) {
                cerr << "Failed to write remaining block " << it->first << endl;
            }
            result_buffer.erase(it);
            m_blocks_written++;
        }
    }
    PerformanceProfiler::end_section("Results Processing");
}

bool CsvProcessor::writeHeader() {
    try {
        const auto& header = m_parser->getHeader();
        m_output_file << ",";
        for (size_t i = 0; i < header.size(); ++i) {
            m_output_file << header[i];
            if (i < header.size() - 1) {
                m_output_file << ",";
            }
        }
        m_output_file << "\n";
        m_output_file.flush();
        return true;
    }
    catch (const exception& e) {
        cerr << "Error writing header: " << e.what() << endl;
        return false;
    }
}

bool CsvProcessor::writeBlock(const GridBlock& block,
    const vector<vector<Cell>>& processed_rows) {
    try {
        lock_guard<mutex> lock(m_write_mutex);
        for (size_t i = 0; i < block.y_coords.size(); ++i) {
            m_output_file << block.y_coords[i] << ",";
            const auto& row = processed_rows[i];
            for (size_t j = 0; j < row.size(); ++j) {
                m_output_file << scientific << setprecision(15) << row[j].thermocurrent;
                if (j < row.size() - 1) {
                    m_output_file << ",";
                }
            }
            m_output_file << "\n";
        }
        m_output_file.flush();
        return true;
    }
    catch (const exception& e) {
        cerr << "Error writing block: " << e.what() << endl;
        return false;
    }
}

void CsvProcessor::print_profiling_report() {
    auto end_time = chrono::high_resolution_clock::now();
    m_stats.processing_time = chrono::duration_cast<chrono::milliseconds>(end_time - m_start_time);
    cout << "Processing time: " << m_stats.processing_time.count() << " ms" << endl;
    MemoryProfiler::print_report();
    PerformanceProfiler::print_report();
}