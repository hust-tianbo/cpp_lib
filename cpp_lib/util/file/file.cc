#include "cpp_lib/util/file/file.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace cpp_lib {
bool IsFileExist(const std::string& path) {
  std::error_code ec;
  std::filesystem::status(path, ec);
  if (ec) {
    return false;
  }

  // 判断文件是否存在
  if (!std::filesystem::exists(path)) {
    return false;
  }

  // 是否是文件夹
  if (std::filesystem::is_directory(path)) {
    return false;
  }
  return true;
}

bool IsDirExist(const std::string& path) {
  struct stat buf = {0};
  int ret = stat(path.c_str(), &buf);
  if (0 == ret) {
    return S_ISDIR(buf.st_mode);
  }
  return false;
}

bool IsExec(const std::string& path) {
  if (IsFileExist(path)) {
    struct stat st = {0};
    if ((stat(path.c_str(), &st) >= 0) && (st.st_mode > 0) &&
        (S_IEXEC & st.st_mode)) {
      return true;
    }
  }
  return false;
}

bool MakeDir(const std::string& path) {
  // 如果已经存在该目录的话
  if (IsDirExist(path) || IsFileExist(path)) {
    return false;
  }

  std::string path_temp = path;
  size_t found = path_temp.rfind("/");
  if (found == path_temp.size() - 1) {
    path_temp = path_temp.substr(0, path_temp.size() - 1);
    found = path_temp.rfind("/");
  }

  if (found != std::string::npos) {
    std::string base_dir = path_temp.substr(0, found);
    if (MakeDir(base_dir)) {
      return mkdir(path.c_str()) == 0;
    }
  }
}

}  // namespace cpp_lib