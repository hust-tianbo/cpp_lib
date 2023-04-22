#include "cpp_lib/coro/coro.h"

#include <chrono>
#include <vector>

namespace cpp_lib {
std::shared_ptr<Coro> StartCoroFunc(const AnyFunc& func) {
  std::shared_ptr<Coro> coro(new Coro);
  coro->thread = new std::thread(func);
  return coro;
}

void ParallelFor(size_t first, size_t last, size_t partition_size, const ParallelFunc& func) {
  if ((last - first) < partition_size) {
    func(first, last);
    return;
  }

  std::vector<CoroPtr> tasks;
  size_t i = first;
  while (i < last) {
    size_t partition_start = i;
    size_t partition_end = partition_start + partition_size;
    if (partition_end > last) {
      partition_end = last;
    }
    auto partition_func = [&func, partition_start, partition_end]() { func(partition_start, partition_end); };
    tasks.push_back(StartCoroFunc(partition_func));
    i = partition_end;
  }
  for (auto cid : tasks) {
    CoroJoin(cid);
  }
}

void CoroYield() { std::this_thread::yield(); }

void CoroJoin(CoroPtr coro) {
  if (!coro) {
    return;
  }
  if (coro->thread != nullptr) {
    coro->thread->join();
  }
}

void CoroSleep(uint64_t micro_secs) { std::this_thread::sleep_for(std::chrono::microseconds(micro_secs)); }
}