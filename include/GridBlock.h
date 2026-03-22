#ifndef GRIDBLOCK_H
#define GRIDBLOCK_H

#include <vector>
#include <string>
#include <optional>
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/Cell.h"

using namespace std;

class GridBlock {
public:
    vector<vector<Cell>> rows;
    vector<string> y_coords;
    size_t start_global_row_index;

    bool is_first_block = false;
    bool is_last_block = false;

    void initialize(size_t row_count, size_t col_count) {
        rows.resize(row_count);
        y_coords.resize(row_count);
        for (auto& row : rows) {
            row.resize(col_count);
        }
    }

    size_t getColumnCount() const {
        return rows.empty() ? 0 : rows[0].size();
    }

    bool empty() const {
        return rows.empty();
    }

    GridBlock() = default;

    GridBlock(GridBlock&& other) noexcept : rows(move(other.rows)), y_coords(move(other.y_coords)), start_global_row_index(other.start_global_row_index), is_first_block(other.is_first_block), is_last_block(other.is_last_block) {}

    GridBlock& operator=(GridBlock&& other) noexcept {
        if (this != &other) {
            rows = move(other.rows);
            y_coords = move(other.y_coords);
            start_global_row_index = other.start_global_row_index;
            is_first_block = other.is_first_block;
            is_last_block = other.is_last_block;
        }
        return *this;
    }

    GridBlock(const GridBlock&) = delete;
    GridBlock& operator=(const GridBlock&) = delete;
};

#endif