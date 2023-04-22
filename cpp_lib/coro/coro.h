#pragma once

#include <functional>
#include <memory>
#include <thread>

namespace cpp_lib {

struct Coro {
  std::thread* thread = nullptr;
  ~Coro() {
    if (thread && thread->joinable()) {
      thread->detach();
    }
    delete thread;
  }
};

using CoroPtr = std::shared_ptr<Coro>;
using AnyFunc = std::function<void(void)>;
CoroPtr StartCoroFunc(const AnyFunc& func);

using ParallelFunc = std::function<void(size_t, size_t)>;
void ParallelFor(size_t first, size_t last, size_t partition_size, const ParallelFunc& func);
void CoroJoin(CoroPtr coro);
void CoroSleep(uint64_t micro_secs);
void CoroYield();
uint64_t CoroSelfId();

}  // namespace cpp_lib