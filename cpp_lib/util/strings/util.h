#pragma once

#include <string>

namespace cpp_lib {

std::string GetBaseName(const std::string& filename);

std::string StrToLower(std::string_view str);

std::string StrToUpper(std::string_view str);

std::string RandomStr(uint32_t len);
}  // namespace cpp_lib