#pragma once

#include <tbb/concurrent_hash_map.h>

namespace cpp_lib {

template <typename K, typename V>
class ConcurrentMap {
 public:
  // 插入操作，如果key存在，则不进行插入
  void insert(const K& k, const V& v) {
    value_type value(k, v);
    table_.insert(value);
  }

  // tbb没有实现emplace接口，这里兼容
  void emplace(const K& k, const V& v) { insert(k, v); }

  // 插入操作，如果key存在，则进行值替换
  void insert_and_assign(const K& k, const V& v) {
    accessor ac;
    if (table_.find(ac, k)) {
      ac->second = v;
    } else {
      insert(k, v);
    }
  }

  // 查找操作，返回key是否存在(true or false)
  bool find(const K& k) {
    const_accessor cac;
    return table_.find(cac, k);
  }

  // 删除操作，返回是否操作成功
  bool erase(const K& k) {
    accessor ac;
    if (!table_.find(ac, k)) {
      return false;
    }
    return table_.erase(ac);
  }

  // 重载[]符号，返回可写的value
  V& operator[](const K& k) {
    accessor ac;
    table_.insert(ac, k);
    return ac->second;
  }

  // 重载[]符号，不可写，且需要先判断key是否存在，否则会拿到不可预料的结果
  const V& operator[](const K& k) const { return this->at[k]; }

  // 类似map的count方法，判断key是否存在
  int count(const K& k) {
    const_accessor ac;
    return table_.find(ac, k) ? 1 : 0;
  }

  // 类似map的at方法，需要先判断key是否存在，否则会拿到不可预料的结果
  const V& at(const K& k) {
    const_accessor cac;
    table_.find(cac, k);
    return cac->second;
  }

  // 查找并且返回key下面的数据指针，注意需要判断指针是否为空
  std::shared_ptr<V> find_with_value(const K& k) {
    const_accessor cac;
    if (table_.find(cac, k)) {
      return std::make_shared<V>(cac->second);
    } else {
      return nullptr;
    }
  }

  // 返回表里的key数量
  int size() { return table_.size(); }

 private:
  using TableType = tbb::concurrent_hash_map<K, V>;
  using accessor = typename TableType::accessor;
  using const_accessor = typename TableType::const_accessor;
  using value_type = typename TableType::value_type;
  TableType table_;
};
}  // namespace cpp_lib