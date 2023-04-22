#pragma once

#include <memory>
#include <set>
#include <string>

#include "kcfg.hpp"

#include "cpp_lib/container/double_buffer.h"
#include "cpp_lib/reloader/reloadable_file.h"
#include "cpp_lib/serialize/json.h"
#include "cpp_lib/util/file/file.h"

namespace cpp_lib {
template <class T>
class ReloadableJson : public ReloadableFile {
 public:
  int Load(const std::string& path, int64_t length) {
    std::shared_ptr<T> ptr_data = double_confs_.Next();
    std::string json_content;
    std::set<std::string> comments;
    comments.insert("//");
    comments.insert("#");

    FileReadWithoutComment(path, comments, json_content);

    if (!serialize::ParseFromJsonString(json_content, *ptr_data)) {
      return kReloadableConfigParseJsonFailedErrorCode;
    }

    if (!ptr_data->Init()) {
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