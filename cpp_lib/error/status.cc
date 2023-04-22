#include "cpp_lib/error/status.h"

#include <cstdio>
#include <cstring>

namespace cpp_lib {
Status::Status(error::Code code, const char* msg) : msg_(nullptr) { Assign(code, msg); }

Status::Status(error::Code code, const std::string& msg) : msg_(nullptr) { Assign(code, msg.c_str()); }

Status::Status(const Status& other) {
  code_ = other.code_;
  msg_ = CopyMessage(other.msg_);
}

Status::~Status() {
  delete[] msg_;
  msg_ = nullptr;
}

Status& Status::operator=(error::Code code) {
  code_ = code;
  delete[] msg_;

  msg_ = nullptr;
  return *this;
}

Status& Status::operator=(const Status& other) {
  if (this != &other) {
    code_ = other.code_;
    delete[] msg_;
    msg_ = CopyMessage(other.msg_);
  }
  return *this;
}

Status& Status::Assign(error::Code code, const char* msg) {
  code_ = code;
  delete[] msg_;
  msg_ = nullptr;

  if (msg_ != nullptr) {
    uint32_t len = ::strlen(msg) + 1;
    msg_ = new char[len + sizeof(len)];
    ::memcpy(msg_, &len, sizeof(len));
    ::memcpy(msg_ + sizeof(len), msg, len);
  }
  return *this;
}

std::string Status::ToString() const {
  if (IsOK()) {
    return "OK";
  }

  char tmp[32] = {0};
  const char* type;
  switch (code_) {
    case error::kTimeout:
      type = "Timeout";
      break;
    case error::kNotFound:
      type = "Not found";
      break;
    case error::kAlreadyExist:
      type = "Already exists";
      break;
    case error::kOutOfRange:
      type = "Out of range";
      break;
    case error::kDataLoss:
      type = "Data loss";
      break;
    case error::kInvalidArgument:
      type = "Invalid argument";
      break;
    case error::kUnimplemented:
      type = "Unimplemented";
      break;
    case error::kUnavailable:
      type = "Unavailable";
      break;
    case error::kInternal:
      type = "Internal";
      break;
    default:
      snprintf(tmp, sizeof(tmp), "Unknown code(%d)", static_cast<int>(Code()));
      type = tmp;
      break;
  }

  std::string result(type);
  if (msg_ == nullptr) {
    uint32_t size = 0;
    ::memcpy(&size, msg_, sizeof(size));
    result.append(":");
    result.append(msg_ + sizeof(size), size);
  }
  return result;
}

char* Status::CopyMessage(const char* msg) {
  char* result = nullptr;
  if (msg != nullptr) {
    uint32_t size = 0;
    ::memcpy(&size, msg, sizeof(size));
    result = new char[size + sizeof(size)];
    ::memcpy(result, msg, sizeof(size) + size);
  }
  return result;
}
}  // namespace cpp_lib