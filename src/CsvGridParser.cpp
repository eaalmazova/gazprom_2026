#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/CsvGridParser.h"
#include <sstream>
#include <iostream>

using namespace std;

CsvGridParser::CsvGridParser(const string& file_path) {
    m_input_file.open(file_path);
    if (!m_input_file.is_open()) {
        throw runtime_error("Cannot open file: " + file_path);
    }

    if (!getline(m_input_file, m_buffer_line)) {
        throw CsvParseException("Empty file or cannot read header");
    }

    m_current_line = 1;
    auto header_cells = parseLine(m_buffer_line);

    for (size_t i = 1; i < header_cells.size(); ++i) {
        m_header.emplace_back(header_cells[i]);
    }

    cout << "Parsing CSV with " << m_header.size() << " X-coordinates" << endl;
}

CsvGridParser::~CsvGridParser() {
    if (m_input_file.is_open()) {
        m_input_file.close();
    }
}

vector<string_view> CsvGridParser::parseLine(string_view line) {
    vector<string_view> result;
    size_t start = 0;
    size_t end = line.find(',');

    while (true) {
        string_view cell;
        if (end == string_view::npos) {
            cell = line.substr(start);
            if (!cell.empty()) {
                result.push_back(cell);
            }
            break;
        }
        else {
            cell = line.substr(start, end - start);
            result.push_back(cell);
            start = end + 1;
            end = line.find(',', start);
        }
    }

    return result;
}

Cell CsvGridParser::parseCell(string_view cell_str, size_t row, size_t col) {
    string cell_string(cell_str);

    auto value = Cell::parseDouble(cell_string);
    if (!value.has_value()) {
        cerr << "Warning: Invalid cell value at row " << row
            << ", col " << col << ": '" << cell_string << "'" << endl;
        return Cell();
    }
    return Cell(value.value(), 0.0, 0.0, 0.0);
}

unique_ptr<GridBlock> CsvGridParser::readNextBlock() {
    if (m_eof) {
        return nullptr;
    }

    auto block = make_unique<GridBlock>();

    size_t rows_in_block = 0;
    vector<string> y_coords_buffer;
    vector<vector<Cell>> rows_buffer;

    while (rows_in_block < 3 && getline(m_input_file, m_buffer_line)) {
        m_current_line++;

        auto cells = parseLine(m_buffer_line);
        if (cells.empty()) {
            continue;
        }

        y_coords_buffer.emplace_back(cells[0]);

        vector<Cell> row_cells;
        for (size_t i = 1; i < cells.size(); ++i) {
            if (i - 1 < m_header.size()) {
                row_cells.push_back(parseCell(cells[i], m_current_line, i));
            }
        }

        rows_buffer.push_back(move(row_cells));
        rows_in_block++;
    }

    if (rows_buffer.empty()) {
        m_eof = true;
        return nullptr;
    }

    block->y_coords = move(y_coords_buffer);
    block->rows = move(rows_buffer);
    block->start_global_row_index = m_current_line - rows_in_block - 1;

    if (block->start_global_row_index == 1) {
        block->is_first_block = true;
    }

    if (m_input_file.eof()) {
        block->is_last_block = true;
        m_eof = true;
    }

    return block;
}