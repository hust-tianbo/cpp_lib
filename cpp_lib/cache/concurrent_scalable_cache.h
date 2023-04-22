#pragma once

#include <limits>
#include <memory>
#include <shared_mutex>

#include "cpp_lib/cache/concurrent_lru_cache.h"

namespace cpp_lib {
/**
* ConcurrentScalableCache is a thread-safe sharded hashtable with limited
* size. When it is full, it evicts a rough approximation to the least recently
* used item.
*
* The find() operation fills a ConstAccessor object, which is a smart pointer
* similar to TBB's const_accessor. After eviction, destruction of the value is
* deferred until all ConstAccessor objects are destroyed.
*
* Since the hash value of each key is requested multiple times, you should use
* a key with a memoized hash function. LRUCacheKey is provided for
* this purpose.
*/
template <class TKey, class TValue, class TMutex = std::shared_mutex, class THash = tbb::tbb_hash_compare<TKey>>
struct ConcurrentScalableCache {
  using Shard = ConcurrentLRUCache<TKey, TValue, TMutex, THash>;
  typedef typename Shard::ConstAccessor ConstAccessor;

  /**
   * Constructor
   *   - max_size: the maximum number of items in the container
   *   - num_shards: the number of child containers. If this is zero, the
   *     "hardware concurrency" will be used (typically the logical processor
   *     count).
   *   - timeout: key expired time (with second)
   *   - evict_type: batch or one node(s) evict once
   */
  explicit ConcurrentScalableCache(size_t max_size, uint32_t timeout = 0, size_t num_shards = 0,
                                   cache::CacheEvictType evict_type = cache::kEvictOne);

  ConcurrentScalableCache(const ConcurrentScalableCache&) = delete;
  ConcurrentScalableCache& operator=(const ConcurrentScalableCache&) = delete;

  /**
   * Find a value by key, and return it by filling the ConstAccessor, which
   * can be default-constructed. Returns true if the element was found, false
   * otherwise. Updates the eviction list, making the element the
   * most-recently used. In degenerated version, eviction list not be updated
   * when doing find operation
   */
  bool find(ConstAccessor& ac, const TKey& key);

  /**
   * Insert a value into the container. Both the key and value will be copied.
   * The new element will put into the eviction list as the most-recently
   * used.
   *
   * If there was already an element in the container with the same key,
   * the eviction list will update the particular element with the same key,
   * and true will be returned.
   *
   * If a new element is inserted successfully, true will be returned. Otherwise,
   * false will be returned.
   */
  bool insert(const TKey& key, const TValue& value);

  /**
   * Batch insert
   */
  void insert(const std::unordered_map<TKey, TValue>& data);

  /**
   * Clear the container. NOT THREAD SAFE -- do not use while other threads
   * are accessing the container.
   */
  void clear();

  /**
   * Get a snapshot of the keys in the container by copying them into the
   * supplied vector. This will block inserts and prevent LRU updates while it
   * completes. The keys will be inserted in a random order.
   */
  void snapshot_keys(std::vector<TKey>& keys);

  /**
   * Get the approximate size of the container. May be slightly too low when
   * insertion is in progress.
   */
  size_t size() const;

 private:
  /**
   * Get the child container for a given key
   */
  Shard& get_shard(const TKey& key);

  size_t get_shard_ind(const TKey& key);

  /**
   * The maximum number of elements in the container.
   */
  size_t m_max_size;

  /**
   * The child containers
   */
  size_t m_num_shards;
  typedef std::shared_ptr<Shard> ShardPtr;
  std::vector<ShardPtr> m_shards;
};

template <class TKey, class TValue, class TMutex, class THash>
ConcurrentScalableCache<TKey, TValue, TMutex, THash>::ConcurrentScalableCache(size_t max_size, uint32_t timeout,
                                                                              size_t num_shards,
                                                                              cache::CacheEvictType evict_type)
    : m_max_size(max_size), m_num_shards(num_shards) {
  if (m_num_shards == 0) {
    m_num_shards = std::thread::hardware_concurrency();
  }
  for (size_t i = 0; i < m_num_shards; i++) {
    size_t s = max_size / m_num_shards;
    if (i == 0) {
      s += max_size % m_num_shards;
    }
    m_shards.emplace_back(std::make_shared<Shard>(s, timeout, evict_type));
  }
}

template <class TKey, class TValue, class TMutex, class THash>
size_t ConcurrentScalableCache<TKey, TValue, TMutex, THash>::get_shard_ind(const TKey& key) {
  THash hash_obj;
  constexpr int shift = std::numeric_limits<size_t>::digits - 16;
  size_t h = (hash_obj.hash(key) >> shift) % m_num_shards;
  return h;
}

template <class TKey, class TValue, class TMutex, class THash>
typename ConcurrentScalableCache<TKey, TValue, TMutex, THash>::Shard&
ConcurrentScalableCache<TKey, TValue, TMutex, THash>::get_shard(const TKey& key) {
  size_t h = get_shard_ind(key);
  return *(m_shards.at(h));
}

template <class TKey, class TValue, class TMutex, class THash>
bool ConcurrentScalableCache<TKey, TValue, TMutex, THash>::find(ConstAccessor& ac, const TKey& key) {
  return get_shard(key).find(ac, key);
}

template <class TKey, class TValue, class TMutex, class THash>
bool ConcurrentScalableCache<TKey, TValue, TMutex, THash>::insert(const TKey& key, const TValue& value) {
  return get_shard(key).insert(key, value);
}

template <class TKey, class TValue, class TMutex, class THash>
void ConcurrentScalableCache<TKey, TValue, TMutex, THash>::insert(const std::unordered_map<TKey, TValue>& data) {
  std::vector<std::unordered_map<const TKey*, const TValue*>> data_vec;
  data_vec.resize(m_num_shards);
  for (const auto& pair : data) {
    size_t ind = get_shard_ind(pair.first);
    data_vec[ind].insert(std::make_pair(&(pair.first), &(pair.second)));
  }
  for (size_t i = 0; i < m_num_shards; i++) {
    if (data_vec[i].size() > 0) {
      m_shards[i]->insert(data_vec[i]);
    }
  }
}

template <class TKey, class TValue, class TMutex, class THash>
void ConcurrentScalableCache<TKey, TValue, TMutex, THash>::clear() {
  for (size_t i = 0; i < m_num_shards; i++) {
    m_shards[i]->clear();
  }
}

template <class TKey, class TValue, class TMutex, class THash>
void ConcurrentScalableCache<TKey, TValue, TMutex, THash>::snapshot_keys(std::vector<TKey>& keys) {
  for (size_t i = 0; i < m_num_shards; i++) {
    m_shards[i]->snapshot_keys(keys);
  }
}

template <class TKey, class TValue, class TMutex, class THash>
size_t ConcurrentScalableCache<TKey, TValue, TMutex, THash>::size() const {
  size_t size = 0;
  for (size_t i = 0; i < m_num_shards; i++) {
    size += m_shards[i]->size();
  }
  return size;
}
}  // namespace cpp_lib