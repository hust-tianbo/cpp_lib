#pragma once

#include <string>
#include <vector>

namespace cpp_lib {

// 判断路径文件是否存在
[[nodiscard]] bool IsFileExist(const std::string& path);

// 判断路径目录是否存在
[[nodiscard]] bool IsDirExist(const std::string& path);

// 根据给定路径创建目录
[[nodiscard]] bool MakeDir(const std::string& path);

// 根据给定路径创建文件
[[nodiscard]] bool MakeFile(const std::string& path);

// 遍历目录，获取目录下面的所有文件和子目录
[[nodiscard]] bool ListDirs(const std::string& path,
                            std::vector<std::string>& dirs);

// 遍历目录，只获取其中的文件
[[nodiscard]] bool ListFiles(const std::string& path,
                             std::vector<std::string>& files);

// 遍历目录，获取目录下面的所有文件，包含子目录文件
[[nodiscard]] bool ListAllFiles(const std::string& path,
                                std::vector<std::string> files);

// 获取指定文件的大小
int64_t FileSize(const std::string& path);

// 获取文件的绝对路径
std::string AbsolutePath(const std::string& path);

// 获取当前可执行文件的绝对路径
std::string GetCurrExecFile();

// 文件拷贝
bool FileCopy(const std::string& src, const std::string& dst);

// 目录拷贝
bool DirCopy(const std::string& src, const std::string& dst);

// 得到当前进程所有打开的fd
void GetOpenFds(std::vector<int>& fds);
}  // namespace cpp_lib