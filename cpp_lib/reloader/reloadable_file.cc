#include "cpp_lib/reloader/reloadable_file.h"

#include <sys/mman.h>
#include <sys/stat.h>

#include <atomic>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "cpp_lib/reloader/error_def.h"

namespace cpp_lib {

int MMapData::MMap(FILE* file, int64_t length) {
  if (!Munmap()) {
    return -1;
  }

  void* mdata = mmap(NULL, length, PROT_READ, MAP_SHARED, file->_fileno, 0);
  if (mdata == reinterpret_cast<void*>(-1)) {
    return -1;
  }

  length_ = length;
  data_ = mdata;
  return 0;
}

bool MMapData::Munmap() {
  if (data_ == nullptr) {
    return true;
  }

  if (munmap(data_, length_) == -1) {
    return false;
  }

  data_ = nullptr;
  length_ = 0;
  return true;
}

int ReloadableFile::Init(const std::string& path, LoadCallback* callback) {
  SetCallBack(callback);
  file_info_ = std::make_shared<FileInfo>(path);
  status_ = kInitStatusType;
  return 0;
}

int ReloadableFile::Init(const FileInfoPtr file_info, LoadCallback* callback) {
  SetCallBack(callback);
  file_info_ = file_info;
  status_ = kInitStatusType;
  return 0;
}

#define RETURN_TRYLOAD_FAILED(code)                   \
  if (callback_ != nullptr && callback_->failed_cb) { \
    callback_->failed_cb(code, callback_->user_data); \
  }                                                   \
  return code;

#define RETURN_TRYLOAD_OK()                                \
  if (callback_ != nullptr && callback_->successfull_cb) { \
    callback_->successfull_cb(callback_->user_data);       \
  }                                                        \
  return kStoneOK;

// todo 实现上好像有点问题
int ReloadableFile::TryLoad() {
  if (status_ < kInitStatusType) {
    RETURN_TRYLOAD_FAILED(kReloaderFileUninitializedErrorCode);
  }

  const std::string& path = file_info_->path;
  struct stat st;
  if (stat(path.c_str(), &st) != 0) {
    RETURN_TRYLOAD_FAILED(kReloaderFileStatFileErrorCode);
  }

  // todo 这里实现上
  if (st.st_mtim.tv_sec <= file_info_->update_time) {
    RETURN_TRYLOAD_OK()
  }
  status_ = kLoadingStatusType;
  int ret = Load(path, st.st_size);
  if (ret != kStoneOK) {
    status_ = kLoadFailedStatusType;
    RETURN_TRYLOAD_FAILED(static_cast<StoneErrorCodeType>(ret));
  }
  file_info_->update_time = st.st_mtim.tv_sec;
  status_ = kLoadSucceedStatusType;
  RETURN_TRYLOAD_OK()
}

}  // namespace cpp_lib