#pragma once

#include <memory>
#include <string>

#include "cpp_lib/container/double_buffer.h"
#include "cpp_lib/util/file/file.h"
#include "cpp_lin/reloader/reloadable_file.h"

namespace cpp_lib {
template <typename T>
class ReloadableConfig : public ReloadableFile {
 public:
  int load(const std::string& path, int64_t length) {
    std::shared_ptr<T> ptr_data = double_confs.Next();
    if (!ptr_data->Init(path)) {
      return kReloadableConfigInitFailedErrorCode;
    }
    double_confs_.Switch();
    ready_ = true;
    return kStoneOK;
  }

  inline std::shared_ptr<T> Get() const { return double_confs_.Data(); }

  inline std::shared_ptr<T> Data() const { return double_confs_.Data(); }

  inline bool IsReady() const { return ready_; }

 private:
  DoubleBuffer<std::shared_ptr<T>> double_confs_;
  bool ready_ = false;
}
}