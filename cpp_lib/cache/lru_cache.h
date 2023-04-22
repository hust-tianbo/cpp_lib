#pragma once

#include <shared_mutex>

#include "cpp_lib/cache/concurrent_scalable_cache.h"

namespace cpp_lib {

/**
 * To avoid unnecessary value copy, TValue can be instantiated with std::shared_ptr<type>
 */
template <class TKey, class TValue, class TMutex = std::shared_mutex, class THash = tbb::tbb_hash_compare<TKey>>
class LRUCache {
 public:
  explicit LRUCache(size_t max_size, uint32_t timeout = 0, size_t num_shards = 0,
                    cache::CacheEvictType evict_type = cache::kEvictOne);

  LRUCache(const LRUCache&) = delete;
  LRUCache& operator=(const LRUCache&) = delete;

  // If hit, true will be returned. Otherwise, false will be returned.
  bool get(const TKey& key);

  // If hit, true will be returned and value will be set. Otherwise, false will be returned.
  bool get(const TKey& key, TValue& value);

  // Batch get. Only missing keys (expired or not existed) will be set.
  void mget(const std::vector<TKey>& keys, std::vector<TKey>& not_find_keys);

  // Batch get. Missing keys and hit values will be set.
  void mget(const std::vector<TKey>& keys, std::unordered_map<TKey, TValue>& values, std::vector<TKey>& not_find_keys);

  // If seting pair successfully, true will be returned. Otherwise, false will be returned.
  bool set(const TKey& key, const TValue& value);

  void mset(const std::unordered_map<TKey, TValue>& data);

  size_t size() { return m_cache_->size(); }

 private:
  using Cache = ConcurrentScalableCache<TKey, TValue, TMutex, THash>;
  typedef typename Cache::ConstAccessor ConstAccessor;
  std::shared_ptr<Cache> m_cache_ = nullptr;
};

template <class TKey, class TValue, class TMutex, class THash>
LRUCache<TKey, TValue, TMutex, THash>::LRUCache(size_t max_size, uint32_t timeout, size_t num_shards,
                                                cache::CacheEvictType evict_type) {
  m_cache_ = std::make_shared<Cache>(max_size, timeout, num_shards, evict_type);
}

template <class TKey, class TValue, class TMutex, class THash>
bool LRUCache<TKey, TValue, TMutex, THash>::get(const TKey& key) {
  ConstAccessor ac;
  return m_cache_->find(ac, key);
}

template <class TKey, class TValue, class TMutex, class THash>
bool LRUCache<TKey, TValue, TMutex, THash>::get(const TKey& key, TValue& value) {
  ConstAccessor ac;
  bool flag = m_cache_->find(ac, key);
  if (flag) {
    value = ac.get_value();
  }
  return flag;
}

template <class TKey, class TValue, class TMutex, class THash>
void LRUCache<TKey, TValue, TMutex, THash>::mget(const std::vector<TKey>& keys, std::vector<TKey>& not_find_keys) {
  for (const auto& key : keys) {
    if (!get(key)) {
      not_find_keys.push_back(key);
    }
  }
}

template <class TKey, class TValue, class TMutex, class THash>
void LRUCache<TKey, TValue, TMutex, THash>::mget(const std::vector<TKey>& keys,
                                                 std::unordered_map<TKey, TValue>& values,
                                                 std::vector<TKey>& not_find_keys) {
  for (const auto& key : keys) {
    ConstAccessor ac;
    if (m_cache_->find(ac, key)) {
      values.insert(std::make_pair(key, ac.get_value()));
    } else {
      not_find_keys.push_back(key);
    }
  }
}

template <class TKey, class TValue, class TMutex, class THash>
bool LRUCache<TKey, TValue, TMutex, THash>::set(const TKey& key, const TValue& value) {
  return m_cache_->insert(key, value);
}

template <class TKey, class TValue, class TMutex, class THash>
void LRUCache<TKey, TValue, TMutex, THash>::mset(const std::unordered_map<TKey, TValue>& data) {
  m_cache_->insert(data);
}

}  // namespace cpp_lib