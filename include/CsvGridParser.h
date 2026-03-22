#ifndef CSVGRIDPARSER_H
#define CSVGRIDPARSER_H

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <stdexcept>
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/GridBlock.h"

using namespace std;

class CsvParseException : public runtime_error {
public:
    explicit CsvParseException(const string& message, size_t line_num = 0) : runtime_error(message + (line_num > 0 ? " at line " + to_string(line_num) : "")), m_line_number(line_num) {}
    size_t getLineNumber() const { return m_line_number; }

private:
    size_t m_line_number;
};

class CsvGridParser {
public:
    explicit CsvGridParser(const string& file_path);
    ~CsvGridParser();
    unique_ptr<GridBlock> readNextBlock();
    const vector<string>& getHeader() const { return m_header; }
    bool eof() const { return m_eof; }
    size_t getCurrentLine() const { return m_current_line; }

private:
    vector<string_view> parseLine(string_view line);
    Cell parseCell(string_view cell_str, size_t row, size_t col);
    ifstream m_input_file;
    vector<string> m_header;
    size_t m_current_line = 0;
    bool m_eof = false;
    string m_buffer_line;
};

#endif