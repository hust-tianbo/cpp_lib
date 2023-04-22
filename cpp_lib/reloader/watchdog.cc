#include "cpp_lib/reloader/watchdog.h"

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "cpp_lib/reloader/error_def.h"
#include "cpp_lib/reloader/reloadable_file.h"

namespace cpp_lib {

void ReloadFilesWatchDog::Run() {
  while (true) {
    {
      std::shared_lock<std::shared_mutex> guard(files_lock_);
      if (stop_) {
        break;
      }
      for (auto& file : watched_files_) {
        (file.second)->TryLoad();
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kWatchDogLoopSleepDuringMs));
  }
}

int ReloadFilesWatchDog::Watch(ReloadableFilePtr file) {
  {
    std::shared_lock<std::shared_mutex> guard(files_lock_);
    if (file == nullptr) {
      return -1;
    }

    const std::string& path = file->Path();
    if (watched_files_.count(path) > 0) {
      return -1;
    }
  }
  {
    std::unique_lock<std::shared_mutex> guard(files_lock_);
    file->TryLoad();

    watched_files_[file->Path()] = file;
  }
  return 0;
}

int ReloadFilesWatchDog::Cancel(const std::string& path) {
  std::unique_lock<std::shared_mutex> guard(files_lock_);
  if (watched_files_.count(path) <= 0) {
    return -1;
  }
  watched_files_.erase(path);
  return 0;
}

void ReloadFilesWatchDog::Stop() {
  std::unique_lock<std::shared_mutex> guard(files_lock_);

  if (stop_) {
    return;
  }

  watched_files_.clear();
  stop_ = true;
}

}  // namespace cpp_lib