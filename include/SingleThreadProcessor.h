#ifndef SINGLETHREADPROCESSOR_H
#define SINGLETHREADPROCESSOR_H

#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/CsvGridParser.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/GridBlock.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/NeighborhoodAveragePolicy.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/GridAggregator.h"
#include <fstream>
#include <chrono>
#include <iostream>
#include <iomanip>

using namespace std;

class SingleThreadProcessor {
public:
    SingleThreadProcessor(const string& input_path,
        const string& output_path)
        : m_input_path(input_path)
        , m_output_path(output_path) {
    }

    chrono::milliseconds process() {
        auto start_time = chrono::high_resolution_clock::now();
        CsvGridParser parser(m_input_path);
        ofstream output_file(m_output_path);
        const auto& header = parser.getHeader();
        output_file << ",";
        for (size_t i = 0; i < header.size(); ++i) {
            output_file << header[i];
            if (i < header.size() - 1) output_file << ",";
        }
        output_file << "\n";

        GridAggregator<> aggregator;

        size_t block_count = 0;
        while (true) {
            auto block = parser.readNextBlock();
            if (!block) break;

            auto processed_rows = aggregator.processBlock(*block);

            for (size_t i = 0; i < block->y_coords.size(); ++i) {
                output_file << block->y_coords[i] << ",";
                const auto& row = processed_rows[i];
                for (size_t j = 0; j < row.size(); ++j) {
                    output_file << scientific << setprecision(15)
                        << row[j].thermocurrent;
                    if (j < row.size() - 1) output_file << ",";
                }
                output_file << "\n";
            }

            block_count++;
        }

        output_file.close();

        auto end_time = chrono::high_resolution_clock::now();
        return chrono::duration_cast<chrono::milliseconds>(
            end_time - start_time);
    }

private:
    string m_input_path;
    string m_output_path;
};

#endif