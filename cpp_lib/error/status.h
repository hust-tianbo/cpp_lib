#pragma once

#include <string>

namespace cpp_lib {
namespace error {
enum Code {
  kOK = 0,
  kTimeout = 1,
  kNotFound = 2,
  kAlreadyExist = 3,
  kOutOfRange = 4,
  kDataLoss = 5,
  kInvalidArgument = 6,
  kUnimplemented = 7,
  kUnavailable = 8,
  kInternal = 20,
};
}  // namespace error

class Status {
 public:
  explicit Status(error::Code code = error::kOK, const char* msg = nullptr);
  Status(error::Code code, const std::string& msg);
  Status(const Status& s);
  ~Status();

  Status& operator=(error::Code code);
  Status& operator=(const Status& s);
  Status& Assign(error::Code code, const char* msg = nullptr);

  static Status OK() { return Status(); }

  bool IsOK() const { return code_ == error::kOK; }

  error::Code Code() const { return code_; }

  std::string ToString() const;

 private:
  char* CopyMessage(const char* msg);

  bool operator==(const Status& other);

 private:
  error::Code code_ = error::kOK;
  char* msg_ = nullptr;
};
}