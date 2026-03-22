#ifndef GRIDAGGREGATOR_H
#define GRIDAGGREGATOR_H

#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/NeighborhoodAveragePolicy.h"
#include "C:/Users/Almaz/source/repos/ConsoleApplication3/include/GridBlock.h"
#include <vector>
#include <memory>

using namespace std;

template<typename AggregationPolicy = NeighborhoodAveragePolicy<>>
class GridAggregator : private AggregationPolicy {
public:
    using AggregationPolicy::apply;
    GridAggregator() = default;
    explicit GridAggregator(AggregationPolicy policy) : AggregationPolicy(move(policy)) {}
    vector<vector<Cell>> processBlock(const GridBlock& block) {
        vector<vector<Cell>> processed_rows;
        processed_rows.reserve(block.rows.size());
        for (size_t i = 0; i < block.rows.size(); ++i) {
            vector<Cell> result_row;
            const vector<Cell>& top_row = getTopRow(block, i);
            const vector<Cell>& bottom_row = getBottomRow(block, i);
            this->apply(top_row, block.rows[i], bottom_row, result_row);
            processed_rows.push_back(move(result_row));
        }
        return processed_rows;
    }
    
    const AggregationPolicy& getPolicy() const {
        return static_cast<const AggregationPolicy&>(*this);
    }
    
    void setPolicy(const AggregationPolicy& policy) {
        static_cast<AggregationPolicy&>(*this) = policy;
    }

private:
    const vector<Cell>& getTopRow(const GridBlock& block, size_t row_idx) const {
        static const vector<Cell> empty_row;
        if (row_idx == 0) {
            return empty_row;
        }
        return block.rows[row_idx - 1];
    }
    
    const vector<Cell>& getBottomRow(const GridBlock& block, size_t row_idx) const {
        static const vector<Cell> empty_row;
        if (row_idx + 1 >= block.rows.size()) {
            return empty_row;
        }
        return block.rows[row_idx + 1];
    }
};

#endif