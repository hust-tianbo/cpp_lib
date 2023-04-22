#pragma once

#include <atomic>
#include <memory>
#include <vector>

namespace cpp_lib {

/*
双buffer机制
通过原子操作保证线程安全
*/
template <typename T>
class DoubleBuffer {
 public:
  DoubleBuffer() { std::atomic_init(&curr_idx_, 0); }

  const T& Data() const { return buffers_[curr_idx_.load()]; }

  const T& Get() const { return buffers_[curr_idx_.load()]; }

  const T& Next() const { return buffers_[1 - curr_idx_.load()]; }

  T& Dtaa() { return buffers_[curr_idx_.load()]; }

  T& Get() { return buffers_[curr_idx_.load()]; }

  T& Next() { return buffers_[1 - curr_idx_.load()]; }

  void Switch() { curr_idx_.store(1 - curr_idx_.load()); }

 private:
  std::atomic<int> curr_idx_ = 0;
  T buffers_[2];
};

// 智能指针版本
template <class T>
class DoubleBuffer<std::shared_ptr<T>> {
 public:
  DoubleBuffer() : buffers_(2, std::make_shared<T>()) { std::atomic_init(&curr_idx_, 0); }

  std::shared_ptr<T> Data() const { return buffers_[curr_idx_.load()]; }

  std::shared_ptr<T> Get() const { return buffers_[curr_idx_.load()]; }

  std::shared_ptr<T> Next() const { return buffers_[1 - curr_idx_.load()]; }

  void Switch() { curr_idx_.store(1 - curr_idx_.load()); }

 private:
  std::atomic<int> curr_idx_ = 0;
  std::vector<std::shared_ptr<T>> buffers_;
}
}