#ifndef NEIGHBORHOOD_AVERAGE_POLICY_H
#define NEIGHBORHOOD_AVERAGE_POLICY_H

#include <vector>
#include <cmath>
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/Cell.h"

using namespace std;

template<typename ValueExtractor = double(*)(const Cell&)>
class NeighborhoodAveragePolicy {
private:
    ValueExtractor value_extractor;

public:
    NeighborhoodAveragePolicy()
        : value_extractor([](const Cell& c) { return c.thermocurrent; }) {
    }

    explicit NeighborhoodAveragePolicy(ValueExtractor extractor)
        : value_extractor(extractor) {
    }

    void apply(const vector<Cell>& top_row,
        const vector<Cell>& curr_row,
        const vector<Cell>& bottom_row,
        vector<Cell>& result_row) const {

        if (curr_row.empty()) {
            return;
        }

        result_row.clear();
        result_row.reserve(curr_row.size());

        const size_t col_count = curr_row.size();

        for (size_t col = 0; col < col_count; ++col) {
            double sum = 0.0;
            int count = 0;
            double current_value = value_extractor(curr_row[col]);
            if (isinf(current_value) || isnan(current_value)) {
                result_row.push_back(curr_row[col]);
                continue;
            }
            if (col > 0) {
                double left_val = value_extractor(curr_row[col - 1]);
                if (!isinf(left_val) && !isnan(left_val)) {
                    sum += left_val;
                    ++count;
                }
            }
            if (col + 1 < col_count) {
                double right_val = value_extractor(curr_row[col + 1]);
                if (!isinf(right_val) && !isnan(right_val)) {
                    sum += right_val;
                    ++count;
                }
            }
            if (!top_row.empty() && col < top_row.size()) {
                double top_val = value_extractor(top_row[col]);
                if (!isinf(top_val) && !isnan(top_val)) {
                    sum += top_val;
                    ++count;
                }
            }
            if (!bottom_row.empty() && col < bottom_row.size()) {
                double bottom_val = value_extractor(bottom_row[col]);
                if (!isinf(bottom_val) && !isnan(bottom_val)) {
                    sum += bottom_val;
                    ++count;
                }
            }
            Cell new_cell;
            new_cell.thermocurrent = (count > 0) ? sum / count : current_value;
            new_cell.thermopower = curr_row[col].thermopower;
            new_cell.resistivity = curr_row[col].resistivity;
            new_cell.temperature = curr_row[col].temperature;
            result_row.push_back(move(new_cell));
        }
    }
    static NeighborhoodAveragePolicy createForThermopower() {
        return NeighborhoodAveragePolicy([](const Cell& c) { return c.thermopower; });
    }

    static NeighborhoodAveragePolicy createForResistivity() {
        return NeighborhoodAveragePolicy([](const Cell& c) { return c.resistivity; });
    }
};

#endif
