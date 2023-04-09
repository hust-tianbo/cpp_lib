#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

namespace cpp_lib {

// url解码
bool UrlDecode(const std::string& in, const std::string& out);

// url参数解析到unordered_map
bool Url2Map(const std::string& url,
             std::unordered_map<std::string, std::string>& args);

}  // namespace cpp_lib