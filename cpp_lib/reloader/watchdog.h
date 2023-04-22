#pragma once

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "cpp_lib/reloader/error_def.h"
#include "cpp_lib/reloader/reloadable_file.h"
#include "cpp_lib/util/singleton.h"

static const int kWatchDogLoopSleepDuringMs = 10 * 1000;

namespace cpp_lib {

// ReloadFilesWatchDog
// 监控文件的变化，并通知具体的加载器重新加载文件内容
class ReloadFilesWatchDog : public Singleton<ReloadFilesWatchDog> {
 public:
  ReloadFilesWatchDog() : stop_(false), thread_(new std::thread([this] { this->Run(); })){};

  ~ReloadFilesWatchDog() {
    Stop();
    if (thread_->joinable()) {
      thread_->join();
    }
  }

  // 循环检查
  void Run();

  // 监听具体的文件
  int Watch(ReloadableFilePtr file);

  // 取消对文件的监听
  int Cancel(const std::string& path);

  // 取消检查线程
  void Stop();

 private:
  bool stop_;
  std::unique_ptr<std::thread> thread_;
  mutable std::shared_mutex files_lock_;

  // 文件加载路径
  std::unordered_map<std::string, ReloadableFilePtr> watched_files_;
};

}  // namespace cpp_lib