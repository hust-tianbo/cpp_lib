#include "cpp_lib/util/file/file.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <cstring>

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
      return mkdir(path.c_str(),
                   S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0;
    }
  } else {
    return mkdir(path.c_str(),
                 S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0;
  }
  return false;
}

bool MakeFile(const std::string& path) {
  // 如果已经存在该目录的话
  if (IsDirExist(path) || IsFileExist(path)) {
    return false;
  }

  std::string path_temp = path;
  size_t found = path_temp.rfind("/");
  if (found != std::string::npos) {
    std::string base_dir = path.substr(0, found);
    if (MakeDir(base_dir)) {
      return open(path.c_str(), O_CREAT,
                  S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    } else {
      return open(path.c_str(), O_CREAT,
                  S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    }
  }
  return false;
}

bool ListDir(const std::string& path, std::vector<std::string>& dirs) {
  struct stat buf = {0};
  int ret = stat(path.c_str(), &buf);

  if (0 == ret) {
    if (S_ISDIR(buf.st_mode)) {
      DIR* dir = opendir(path.c_str());
      if (nullptr != dir) {
        struct dirent* ptr = nullptr;
        while ((ptr = readdir(dir)) != nullptr) {
          if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, "..")) {
            continue;
          }

          std::string file_path = path;
          file_path.append("/").append(ptr->d_name);

          memset(&buf, 0, sizeof(buf));
          ret = stat(path.c_str(), &buf);

          if (ret == 0) {
            if (S_ISDIR(buf.st_mode)) {
              dirs.emplace_back(file_path);
            }
          }
        }
        closedir(dir);
        return true;
      }
    }
  }
  return false;
}

bool ListFiles(const std::string& path, std::vector<std::string>& files) {
  struct stat buf = {0};
  int ret = stat(path.c_str(), &buf);

  if (0 == ret) {
    if (S_ISDIR(buf.st_mode)) {
      DIR* dir = opendir(path.c_str());
      if (nullptr != dir) {
        struct dirent* ptr = nullptr;
        while ((ptr = readdir(dir)) != nullptr) {
          if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, "..")) {
            continue;
          }

          std::string file_path = path;
          file_path.append("/").append(ptr->d_name);

          memset(&buf, 0, sizeof(buf));
          ret = stat(path.c_str(), &buf);

          if (ret == 0) {
            if (S_ISREG(buf.st_mode)) {
              files.emplace_back(file_path);
            }
          }
        }
        closedir(dir);
        return true;
      }
    }
  }
  return false;
}

bool RecursiveListAllFiles(const std::string& path, const std::string& base,
                           std::vector<std::string>& fs) {
  struct stat buf = {0};
  int ret = stat(path.c_str(), &buf);
  if (ret < 0 || !S_ISDIR(buf.st_mode)) {
    return false;
  }
  DIR* dir = opendir(path.c_str());
  if (nullptr == dir) {
    return false;
  }
  struct dirent* ptr = nullptr;
  while ((ptr = readdir(dir)) != nullptr) {
    if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, "..")) {
      continue;
    }
    std::string file_path = path;
    file_path.append("/").append(ptr->d_name);
    memset(&buf, 0, sizeof(buf));
    ret = stat(file_path.c_str(), &buf);
    if (ret == 0) {
      if (S_ISREG(buf.st_mode)) {
        fs.push_back(base + ptr->d_name);
      } else if (S_ISDIR(buf.st_mode)) {
        RecursiveListAllFiles(file_path, base + ptr->d_name + "/", fs);
      }
    }
  }
  closedir(dir);
  return true;
}

bool ListAllFiles(const std::string& path, std::vector<std::string> files) {
  return RecursiveListAllFiles(path, "", files);
}

int64_t FileSize(const std::string& path) {
  struct stat buf = {0};
  int ret = stat(path.c_str(), &buf);
  int64_t filesize = 0;
  if (0 == ret) {
    if (S_ISREG(buf.st_mode)) {
      return buf.st_size;
    } else if (S_ISDIR(buf.st_mode)) {
      DIR* dir = opendir(path.c_str());
      if (nullptr != dir) {
        struct dirent* ptr = nullptr;
        while ((ptr = readdir(dir)) != nullptr) {
          if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, "..")) {
            continue;
          }
          std::string file_path = path;
          file_path.append("/").append(ptr->d_name);
          filesize += FileSize(file_path);
        }
        closedir(dir);
      }
    }
  }
  return filesize;
}

std::string AbsolutePath(const std::string& path) {
  char* tmp = realpath(path.c_str(), nullptr);
  std::string newpath(tmp);
  free(tmp);
  return newpath;
}

std::string GetCurrentExecuteFile() {
  char procpath[256] = {0};
  snprintf(procpath, sizeof(procpath) - 1, "/proc/%u/exe", getpid());
  char ss[1024] = {0};
  uint32_t len = readlink(procpath, ss, sizeof(ss));
  if (len >= sizeof(ss)) {
    len = sizeof(ss);
  }
  return std::string(ss, len);
}

bool FileCopy(const std::string& src, const std::string& dst) {
  int fd_to = 0, fd_from = 0;
  char buf[4096] = {0};
  ssize_t nread = 0;
  int saved_errno = 0;

  fd_from = open(src.c_str(), O_RDONLY);
  if (fd_from < 0) {
    return false;
  }

  remove(dst.c_str());
  fd_to = open(dst.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0666);
  if (fd_to < 0) {
    goto out_error;
  }

  while (nread = read(fd_from, buf, sizeof buf), nread > 0) {
    char* out_ptr = buf;
    ssize_t nwritten = 0;

    do {
      nwritten = write(fd_to, out_ptr, nread);

      if (nwritten >= 0) {
        nread -= nwritten;
        out_ptr += nwritten;
      } else if (errno != EINTR) {
        goto out_error;
      }
    } while (nread > 0);
  }

  if (nread == 0) {
    if (close(fd_to) < 0) {
      fd_to = -1;
      goto out_error;
    }
    close(fd_from);

    /* Success! */
    return true;
  }

out_error:
  saved_errno = errno;

  close(fd_from);
  if (fd_to >= 0) close(fd_to);

  errno = saved_errno;
  return false;
}

bool DirCopy(const std::string& src, const std::string& dst) {
  std::vector<std::string> fs;
  if (!ListAllFiles(src, fs)) {
    return false;
  }
  for (size_t i = 0; i < fs.size(); i++) {
    if (!MakeFile(dst + "/" + fs[i])) {
      return false;
    }
    std::string src_file = src + "/" + fs[i];
    std::string dst_file = dst + "/" + fs[i];
    if (!FileCopy(src_file, dst_file)) {
      return false;
    }
  }
  return true;
}

void GetOpenFds(std::vector<int>& fds) {
  char procpath[256] = {0};
  snprintf(procpath, sizeof(procpath) - 1, "/proc/%u/fd", getpid());
  std::vector<std::string> fs;
  DIR* dir = opendir(procpath);
  if (nullptr != dir) {
    struct dirent* ptr = nullptr;
    while ((ptr = readdir(dir)) != nullptr) {
      if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, "..")) {
        continue;
      }
      fs.push_back(ptr->d_name);
    }
    closedir(dir);
  }
  for (size_t i = 0; i < fs.size(); i++) {
    fds.push_back(atoi(fs[i].c_str()));
  }
}

}  // namespace cpp_lib