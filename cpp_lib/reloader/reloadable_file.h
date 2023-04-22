#pragma once

#include <sys/mman.h>

#include <atomic>
#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <vector>

// 可以热加载的文件
namespace cpp_lib {
struct FileInfo {
 public:
  enum FileType {
    kDictType = 0,
    kJsonType = 1,
    kProtoBufferType = 2,
    kTomlType = 3,
  };

  explicit FileInfo(const std::string& _path) : path(_path) {}

  FileInfo(const std::string& _path, FileType file_type) : path(_path), type(file_type) {}

 public:
  std::string path;
  FileType type = kDictType;
  int64_t update_time = 0;
};

typedef std::shared_ptr<FileInfo> FileInfoPtr;

// 负责mmap区段
class MMapData {
 public:
  ~MMapData() {
    if (!Munmap()) {
    }
  }

  // 实现文件映射
  int MMap(FILE* file, int64_t length);

  void* Data() const { return data_; }

  int64_t Length() const { return length_; }

  bool Munmap();

 private:
  void* data_ = nullptr;
  int64_t length_ = 0;
};

// 文件加载的回调机制
// 包括成功回调，失败回调等
struct LoadCallback {
  std::function<void(void*)> successfull_cb;
  std::function<void(const int, void*)> failed_cb;

  void* user_data = nullptr;
};

// 可加载的文件实现
class ReloadableFile {
 public:
  enum StatusType {
    kUnInitStatusType = 0,
    kInitStatusType,
    kLoadingStatusType,
    kLoadFailedStatusType,
    kLoadSucceedStatusType,
  };

  virtual ~ReloadableFile(){};

  // 通过文件路径初始化
  [[nodiscard]] int Init(const std::string& path, LoadCallback* callback = nullptr);

  // 通过FileInfo初始化
  [[nodiscard]] int Init(const FileInfoPtr file_info, LoadCallback* callback = nullptr);

  // 设置加载成功失败回调函数
  inline void SetCallBack(LoadCallback* callback = nullptr) {
    if (callback != nullptr) {
      callback_ = callback;
    }
  }

  // 文件load
  int TryLoad();

  [[nodiscard]] StatusType Status() const { return status_; }

  // 设置文件路径
  const std::string& Path() const { return file_info_->path; }

  virtual bool IsReady() const = 0;

 private:
  virtual int Load(const std::string& path, int64_t length) = 0;

 private:
  StatusType status_ = kUnInitStatusType;
  FileInfoPtr file_info_;
  LoadCallback* callback_ = nullptr;
};

using ReloadableFilePtr = std::shared_ptr<ReloadableFile>;
}