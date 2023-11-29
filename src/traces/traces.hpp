#pragma once

#include <vector>

#include "../common/logger.hpp"
#include "../common/status.hpp"
#include "../common/types.hpp"

enum class TraceOperator {
    insert,
    search,
    remove,
};

struct Trace {
    TraceOperator op;
    KeyType key;
    ValueType value;

    void
    print() const;
};

/// @brief  Create a trace of elements following the Zipfian distribution with
///         a uniformly random distribution of insert, search, and remove
///         operations.
std::vector<Trace>
generate_random_traces(const size_t max_num_unique_elements,
                       const size_t trace_length,
                       const double insert_ratio = 33.0,
                       const double search_ratio = 33.0,
                       const double remove_ratio = 33.0);
