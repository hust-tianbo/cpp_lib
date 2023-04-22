#pragma once

#include <tbb/concurrent_hash_map.h>
#include <time.h>

#include <atomic>
#include <mutex>
#include <new>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "cpp_lib/coro/coro.h"

namespace cpp_lib {

namespace cache {
static constexpr int kMaxEvictBatch = 64;

enum CacheEvictType {
  kEvictBatch = 0,  // evict batch items once which are expired or overload
  kEvictOne = 1,    // evict only one item once which is expired or overload
};
}  // namespace cache

template <class TKey, class TValue, class TMutex = std::shared_mutex, class THash = tbb::tbb_hash_compare<TKey>>
struct ConcurrentLRUCache {
 private:
  /**
   * The LRU list node.
   *
   * We make a copy of the key in the list node, allowing us to find the
   * TBB::CHM element from the list node. TBB::CHM invalidates iterators
   * on most operations, even find(), ruling out more efficient
   * implementations.
   */
  struct ListNode {
    ListNode() : m_prev(kOutOfListMarker), m_next(nullptr) {}

    explicit ListNode(const TKey& key) : m_key(key), m_prev(kOutOfListMarker), m_next(nullptr) { update_timestamp(); }

    TKey m_key;
    time_t m_timestamp;
    ListNode* m_prev;
    ListNode* m_next;

    bool is_in_list() const { return m_prev != kOutOfListMarker; }

    void update_timestamp() { time(&m_timestamp); }
  };

  static ListNode* const kOutOfListMarker;

  /**
   * The value that we store in the hashtable. The list node is allocated from
   * an internal object_pool. The ListNode* is owned by the list.
   */
  struct HashMapValue {
    HashMapValue() : m_list_node(nullptr) {}
    HashMapValue(const TValue& value, ListNode* node) : m_value(value), m_list_node(node) {}

    TValue m_value;
    ListNode* m_list_node;
  };

  typedef tbb::concurrent_hash_map<TKey, HashMapValue, THash> HashMap;
  typedef typename HashMap::const_accessor HashMapConstAccessor;
  typedef typename HashMap::accessor HashMapAccessor;
  typedef typename HashMap::value_type HashMapValuePair;

 public:
  /**
   * The proxy object for TBB::CHM::const_accessor. Provides direct access to
   * the user's value by dereferencing, thus hiding our implementation
   * details.
   */
  struct ConstAccessor {
    ConstAccessor() {}

    const TValue& operator*() const { return get_value(); }

    // ac->func == (*(ac.operator->())).func
    const TValue* operator->() const { return get_value_ptr(); }

    const TValue* get_value_ptr() const { return &(m_hash_accessor->second.m_value); }

    const TValue& get_value() const { return m_hash_accessor->second.m_value; }

    time_t get_timestamp() const { return (m_hash_accessor->second.m_list_node)->m_timestamp; }

    bool empty() const { return m_hash_accessor.empty(); }

   private:
    friend struct ConcurrentLRUCache;
    HashMapConstAccessor m_hash_accessor;
  };

  /**
   * Create a container with a given maximum size, expired time(with second) and evict type
   */
  explicit ConcurrentLRUCache(size_t max_size, uint32_t timeout = 0,
                              cache::CacheEvictType evict_type = cache::kEvictOne);

  ConcurrentLRUCache(const ConcurrentLRUCache& other) = delete;
  ConcurrentLRUCache& operator=(const ConcurrentLRUCache&) = delete;

  ~ConcurrentLRUCache() { clear(); }

  /**
   * Find a value by key, and return it by filling the ConstAccessor, which
   * can be default-constructed. Returns true if the element was found, false
   * otherwise. Updates the eviction list, making the element the
   * most-recently used.
   */
  bool find(ConstAccessor& ac, const TKey& key);

  /**
   * Insert a value into the container. Both the key and value will be copied.
   * The new element will put into the eviction list as the most-recently
   * used.
   *
   * If there was already an element in the container with the same key, it
   * will be updated.
   */
  bool insert(const TKey& key, const TValue& value);

  /**
   * Batch insert
   */
  void insert(const std::unordered_map<TKey, TValue>& data);

  void insert(const std::unordered_map<const TKey*, const TValue*>& data);

  /**
   * Clear the container. NOT THREAD SAFE -- do not use while other threads
   * are accessing the container.
   */
  void clear();

  /**
   * Get a snapshot of the keys in the container by copying them into the
   * supplied vector. This will block inserts and prevent LRU updates while it
   * completes. The keys will be inserted in order from most-recently used to
   * least-recently used.
   */
  void snapshot_keys(std::vector<TKey>& keys);

  /**
   * Get the approximate size of the container. May be slightly too low when
   * insertion is in progress.
   */
  size_t size() const { return m_size.load(); }

 private:
  /**
   * Unlink a node from the list. The caller must lock the list mutex while
   * this is called.
   */
  void delink(ListNode* node);

  /**
   * Add a new node to the list in the most-recently used position. The caller
   * must lock the list mutex while this is called.
   */
  void push_front(ListNode* node);

  /**
   * Evict the least-recently used item from the container. This function does
   * its own locking.
   */
  void evict();

  /**
   * Insert one node to CHM, if existed, update it
   */
  bool set_entry(const TKey& key, const TValue& value);

  /**
   * Remove last node in CHM
   */
  void remove_node(bool timeout_check = false);

  /**
   * Check last node is expired or not
   */
  bool is_last_node_timeout();

  /**
   * The maximum number of elements in the container.
   */
  size_t m_max_size;

  /**
   * This atomic variable is used to signal to all threads whether or not
   * eviction should be done on insert. It is approximately equal to the
   * number of elements in the container.
   */
  std::atomic<size_t> m_size;

  /**
   * The underlying TBB hash map.
   */
  HashMap m_map;

  /**
   * Cache timeout, key grained
   */
  uint32_t m_timeout;

  /**
   * Cache evict type, batch or one node(s) once
   */
  cache::CacheEvictType m_evict_type;

  /**
   * In evict process flag
   */
  std::atomic<bool> m_evict_flag;

  /**
   * The linked list. The "head" is the most-recently used node, and the
   * "tail" is the least-recently used node. The list mutex must be held
   * during both read and write.
   */
  ListNode m_head;
  ListNode m_tail;
  typedef TMutex ListMutex;
  ListMutex m_list_mutex;
};

template <class TKey, class TValue, class TMutex, class THash>
typename ConcurrentLRUCache<TKey, TValue, TMutex, THash>::ListNode* const
    ConcurrentLRUCache<TKey, TValue, TMutex, THash>::kOutOfListMarker = reinterpret_cast<ListNode*>(-1);

template <class TKey, class TValue, class TMutex, class THash>
ConcurrentLRUCache<TKey, TValue, TMutex, THash>::ConcurrentLRUCache(size_t max_size, uint32_t timeout,
                                                                    cache::CacheEvictType evict_type)
    : m_max_size(max_size),
      m_size(0),
      m_timeout(timeout),
      m_evict_type(evict_type),
      m_evict_flag(false),
      m_map(std::thread::hardware_concurrency() * 4) {
  m_head.m_prev = nullptr;
  m_head.m_next = &m_tail;
  m_tail.m_prev = &m_head;
}

template <class TKey, class TValue, class TMutex, class THash>
bool ConcurrentLRUCache<TKey, TValue, TMutex, THash>::find(ConstAccessor& ac, const TKey& key) {
  HashMapConstAccessor& hash_accessor = ac.m_hash_accessor;
  if (!m_map.find(hash_accessor, key)) {
    return false;
  }
  time_t cur_time;
  time(&cur_time);
  if (m_timeout != 0 && cur_time - ac.get_timestamp() > m_timeout) {
    // key is expired
    return false;
  }
  return true;

  // Fake LRU, don't adjust list node address when doing get operation

  // Acquire the lock, but don't block if it is already held
  // std::lock_guard<ListMutex> lock(m_list_mutex);
  //   ListNode* node = hash_accessor->second.m_list_node;
  //   // The list node may be out of the list if it is in the process of being
  //   // inserted or evicted. Doing this check allows us to lock the list for
  //   // shorter periods of time.
  //   if (node && node->is_in_list()) {
  //     delink(node);
  //     push_front(node);
  //   }
}

template <class TKey, class TValue, class TMutex, class THash>
bool ConcurrentLRUCache<TKey, TValue, TMutex, THash>::set_entry(const TKey& key, const TValue& value) {
  // Insert into the CHM
  HashMapAccessor hash_accessor;
  bool new_flag = m_map.insert(hash_accessor, key);
  if (!new_flag) {
    // Key already exist, update value and timestamp and adjust node address
    hash_accessor->second.m_value = value;
    {
      std::lock_guard<ListMutex> lock(m_list_mutex);
      ListNode* node = hash_accessor->second.m_list_node;
      if (node && node->is_in_list()) {
        node->update_timestamp();
        delink(node);
        push_front(node);
      }
    }
  } else {
    // Insert new node
    ListNode* node = new ListNode(key);
    hash_accessor->second.m_value = value;
    hash_accessor->second.m_list_node = node;

    // Note that we have to update the LRU list before we increment m_size, so
    // that other threads don't attempt to evict list items before they even
    // exist.
    {
      std::lock_guard<ListMutex> lock(m_list_mutex);
      push_front(node);
    }
    m_size++;
  }
  return true;
}

template <class TKey, class TValue, class TMutex, class THash>
bool ConcurrentLRUCache<TKey, TValue, TMutex, THash>::insert(const TKey& key, const TValue& value) {
  bool flag = set_entry(key, value);
  if (flag) {
#ifdef TEST_MODE
    evict();
#else
    auto evict_func = [this]() { this->evict(); };
    StartCoroFunc(evict_func);  // async evict node
#endif
  }
  return flag;
}

template <class TKey, class TValue, class TMutex, class THash>
void ConcurrentLRUCache<TKey, TValue, TMutex, THash>::insert(const std::unordered_map<TKey, TValue>& data) {
  for (const auto& pair : data) {
    set_entry(pair.first, pair.second);
  }
#ifdef TEST_MODE
  evict();
#else
  auto evict_func = [this]() { this->evict(); };
  StartCoroFunc(evict_func);  // async evict node
#endif
}

template <class TKey, class TValue, class TMutex, class THash>
void ConcurrentLRUCache<TKey, TValue, TMutex, THash>::insert(
    const std::unordered_map<const TKey*, const TValue*>& data) {
  for (const auto& pair : data) {
    set_entry(*(pair.first), *(pair.second));
  }
#ifdef TEST_MODE
  evict();
#else
  auto evict_func = [this]() { this->evict(); };
  StartCoroFunc(evict_func);  // async evict node
#endif
}

template <class TKey, class TValue, class TMutex, class THash>
void ConcurrentLRUCache<TKey, TValue, TMutex, THash>::clear() {
  m_map.clear();
  ListNode* node = m_head.m_next;
  ListNode* next;
  while (node != &m_tail) {
    next = node->m_next;
    delete node;
    node = next;
  }
  m_head.m_next = &m_tail;
  m_tail.m_prev = &m_head;
  m_size = 0;
}

template <class TKey, class TValue, class TMutex, class THash>
void ConcurrentLRUCache<TKey, TValue, TMutex, THash>::snapshot_keys(std::vector<TKey>& keys) {
  keys.reserve(keys.size() + m_size.load());
  {
    std::shared_lock<ListMutex> lock(m_list_mutex);
    for (ListNode* node = m_head.m_next; node != &m_tail; node = node->m_next) {
      keys.push_back(node->m_key);
    }
  }
}

template <class TKey, class TValue, class TMutex, class THash>
inline void ConcurrentLRUCache<TKey, TValue, TMutex, THash>::delink(ListNode* node) {
  ListNode* prev = node->m_prev;
  ListNode* next = node->m_next;
  prev->m_next = next;
  next->m_prev = prev;
  node->m_prev = kOutOfListMarker;
}

template <class TKey, class TValue, class TMutex, class THash>
inline void ConcurrentLRUCache<TKey, TValue, TMutex, THash>::push_front(ListNode* node) {
  ListNode* old_real_head = m_head.m_next;
  node->m_prev = &m_head;
  node->m_next = old_real_head;
  old_real_head->m_prev = node;
  m_head.m_next = node;
}

template <class TKey, class TValue, class TMutex, class THash>
void ConcurrentLRUCache<TKey, TValue, TMutex, THash>::remove_node(bool timeout_check) {
  ListNode* moribund = nullptr;
  {
    std::lock_guard<ListMutex> lock(m_list_mutex);
    moribund = m_tail.m_prev;
    if (moribund == &m_head) {
      // List is empty, can't evict
      return;
    }
    if (timeout_check) {
      time_t cur_time;
      time(&cur_time);
      if (cur_time - moribund->m_timestamp <= m_timeout) {
        // Last node is not timeout
        return;
      }
    }
    delink(moribund);
  }

  HashMapAccessor hash_accessor;
  if (!m_map.find(hash_accessor, moribund->m_key)) {
    // Presumably unreachable
    return;
  }
  m_map.erase(hash_accessor);
  delete moribund;
  moribund = nullptr;
  m_size--;
}

template <class TKey, class TValue, class TMutex, class THash>
bool ConcurrentLRUCache<TKey, TValue, TMutex, THash>::is_last_node_timeout() {
  if (m_timeout == 0) {
    return false;
  }
  {
    std::shared_lock<ListMutex> lock(m_list_mutex);
    ListNode* moribund = m_tail.m_prev;
    if (moribund == &m_head) {
      // List is empty
      return false;
    }
    time_t cur_time;
    time(&cur_time);
    if (cur_time - moribund->m_timestamp > m_timeout) {
      return true;
    }
  }
  return false;
}

template <class TKey, class TValue, class TMutex, class THash>
void ConcurrentLRUCache<TKey, TValue, TMutex, THash>::evict() {
  bool expect_val = false;
  if (!m_evict_flag.compare_exchange_strong(expect_val, true)) {
    // In evict process
    return;
  }
  if (m_evict_type == cache::kEvictOne) {
    if (m_size.load() > m_max_size) {
      // Overload
      remove_node();
    }
    if (is_last_node_timeout()) {
      remove_node(true);
    }
  } else if (m_evict_type == cache::kEvictBatch) {
    int cnt = 0;
    while (cnt < cache::kMaxEvictBatch && m_size.load() > m_max_size) {
      remove_node();
      cnt++;
    }
    while (cnt < cache::kMaxEvictBatch && is_last_node_timeout()) {
      remove_node(true);
      cnt++;
    }
  }
  m_evict_flag.store(false);
}

}  // namespace cpp_lib