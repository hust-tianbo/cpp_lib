#include "cpp_lib/error/errors.h"

namespace cpp_lib {
namespace error {
#define DEFINE_ERROR(Type) \
  ::cpp_lib::Status Type(const std::string& msg) { return ::cpp_lib::Status(::cpp_lib::error::k##Type, msg); }

DEFINE_ERROR(Timeout)
DEFINE_ERROR(NotFound)
DEFINE_ERROR(AlreadyExist)
DEFINE_ERROR(OutOfRange)
DEFINE_ERROR(DataLoss)
DEFINE_ERROR(InvalidArgument)
DEFINE_ERROR(Unimplemented)
DEFINE_ERROR(Unavailable)
DEFINE_ERROR(Internal)

#undef DEFINE_ERROR
}  // namespace error
}  // namespace cpp_lib