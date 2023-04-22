#pragma once

#include <string>

#include "cpp_lib/error/status.h"

namespace cpp_lib {
namespace error {

#define RETURN_IF_NOT_OK(...)                  \
  do {                                         \
    const ::cpp_lib::Status _ = (__VA_ARGS__); \
    if (!_.IsOK()) return _;                   \
  } while (0)

#define DECLARE_ERROR(Type)                                                                                    \
  extern ::cpp_lib::Status Type(const std::string& msg);                                                       \
  inline bool Is##Type(const ::cpp_lib::Status& status) { return status.Code() == ::cpp_lib::error::k##Type; } \
  template <typename... T>                                                                                     \
  ::cpp_lib::Status Type(T... args) {                                                                          \
    char msg[128];                                                                                             \
    int ret = snprintf(msg, sizeof(msg), args...);                                                             \
    if (ret < 128 && ret > 0) {                                                                                \
      return Type(std::string(msg, ret));                                                                      \
    } else {                                                                                                   \
      return Type("Invalid message format");                                                                   \
    }                                                                                                          \
  }

DECLARE_ERROR(Timeout)
DECLARE_ERROR(NotFound)
DECLARE_ERROR(AlreadyExist)
DECLARE_ERROR(OutOfRange)
DECLARE_ERROR(DataLoss)
DECLARE_ERROR(InvalidArgument)
DECLARE_ERROR(Unimplemented)
DECLARE_ERROR(Unavailable)
DECLARE_ERROR(Internal)

#undef DECLARE_ERROR

}  // namespace error
}  // namespace cpp_lib