#pragma once

#include <string>
#include <unordered_map>

namespace cpp_lib {
typedef enum {
  kStoneOK = 0,

  kReloaderFileInitErrorCode = 60010001,
  kReloaderFileStatFileErrorCode = 60010002,
  kReloaderFileOpenFileErrorCode = 60010003,
  kReloaderFileUninitializedErrorCode = 60010005,
  kMMapDataMMapErrorCode = 60010100,
  kMMapDataMunmapErrorCode = 60010101,
  kDictManagerConfigParseErrorCode = 60010200,
  kReloadableDictVerifyInvalidDataErrorCode = 60010300,
  kReloadableDictVerifyInvalidHashErrorCode = 60010301,
  kReloadableDictVerifyUnknownErrorCode = 60010302,
  kReloadableConfigParseJsonFailedErrorCode = 60010401,
  kReloadableConfigInitFailedErrorCode = 60010402,
} StoneErrorCodeType;

const std::unordered_map<StoneErrorCodeType, std::string> global_stone_error_code_2_error_msg{
    {kStoneOK, "成功！"},
    {kReloaderFileStatFileErrorCode, "reloader文件状态获取失败！"},
    {kReloaderFileOpenFileErrorCode, "reloader文件打开失败！"},
    {kReloaderFileUninitializedErrorCode, "reloader文件未初始化！"},
    {kMMapDataMMapErrorCode, "mmap映射失败！"},
    {kMMapDataMunmapErrorCode, "munmap解除映射失败！"},
    {kDictManagerConfigParseErrorCode, "词典配置文件解析失败！"},
    {kReloadableDictVerifyInvalidDataErrorCode, "词典文件数据校验失败！"},
    {kReloadableDictVerifyInvalidHashErrorCode, "词典文件hash校验失败！"},
    {kReloadableDictVerifyUnknownErrorCode, "词典校验未知错误！"},
    {kReloadableConfigParseJsonFailedErrorCode, "json数据块解析错误！"},
    {kReloadableConfigInitFailedErrorCode, "数据块初始化错误！"},
};
}  // namespace cpp_lib