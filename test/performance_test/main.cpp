#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

#include <ctime>

#include "../../src/common/logger.hpp"
#include "../../src/common/status.hpp"
#include "../../src/common/types.hpp"
#include "../../src/traces/traces.hpp"

#include "../../src/sequential/sequential.hpp"
#include "../../src/parallel/parallel.hpp"

void
run_sequential_performance_test(const std::vector<Trace> &traces)
{
    clock_t start_time, end_time;

    start_time = clock();
    SequentialRobinHoodHashTable hash_table;
    for (auto &t : traces) {
        switch (t.op) {
        case TraceOperator::insert: {
            hash_table.insert(t.key, t.value);
            break;
        }
        case TraceOperator::search: {
            volatile std::optional<ValueType> r = hash_table.search(t.key);
            break;
        }
        case TraceOperator::remove: {
            hash_table.remove(t.key);
            break;
        }
        default: {
            assert(0 && "impossible!");
        }
        }
    }
    end_time = clock();
    double duration_in_seconds = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    std::cout << "Time in sec: " << duration_in_seconds << std::endl;
}

void
run_parallel_worker(ParallelRobinHoodHashTable &hash_table,
                    const std::vector<Trace> &traces, const size_t t_id,
                    const size_t num_workers)
{
    size_t trace_size = traces.size();

    for (size_t i = t_id; i < trace_size; i += num_workers) {
        const Trace &t = traces[i];
        switch (t.op) {
        case TraceOperator::insert: {
            hash_table.insert(t.key, t.value);
            break;
        }
        case TraceOperator::search: {
            // NOTE Marking this as volatile means the compiler will not
            //      optimize this call out.
            volatile std::optional<ValueType> r = hash_table.find(t.key);
            break;
        }
        case TraceOperator::remove: {
            hash_table.remove(t.key);
            break;
        }
        default: {
            assert(false && "impossible!");
        }
        }
    }
}

void
run_parallel_performance_test(const std::vector<Trace> &traces, const size_t num_workers)
{
    clock_t start_time, end_time;
    std::vector<std::thread> workers;

    start_time = clock();
    ParallelRobinHoodHashTable hash_table;
    for (size_t i = 0; i < num_workers; ++i) {
        workers.emplace_back(run_parallel_worker, hash_table, traces, i, num_workers);
    }
    for (auto &w : workers) {
        w.join();
    }
    end_time = clock();
    double duration_in_seconds = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    std::cout << "Time in sec: " << duration_in_seconds << std::endl;
}

int main() {
    std::vector<Trace> traces = generate_traces(100000, 100000000);
    run_sequential_performance_test(traces);
    run_parallel_performance_test(traces, 1);
    run_parallel_performance_test(traces, 2);
    run_parallel_performance_test(traces, 4);
    run_parallel_performance_test(traces, 8);
    run_parallel_performance_test(traces, 16);
    run_parallel_performance_test(traces, 32);

    return 0;
}