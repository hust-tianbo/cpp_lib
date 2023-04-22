#pragma once

namespace cpp_lib {
namespace serialize {
template <typename T>
class HasGetFieldName {
  typedef char one;
  struct two {
    char x[2];
  };
  template <typename C>
  static one test(decltype(&C::GetFieldName));
  template <typename C>
  static two test(...);

 public:
  enum { value = sizeof(test<T>(0)) == sizeof(char) };
};
}  // namespace serialize
}  // namespace cpp_lib