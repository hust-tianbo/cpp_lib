#pragma once

#include <string>
#include <string_view>

namespace cpp_lib {
[[nodiscard]] bool StrContain(std::string_view haystack,
                              std::string_view needle);

[[nodiscard]] bool StrContain(std::string_view haystack, char needle);

[[nodiscard]] bool StartWith(std::string_view text, std::string_view prefix);

[[nodiscard]] bool EndWith(std::string_view text, std::string_view suffix);

[[nodiscard]] bool EqualIgnoreCase(std::string_view piece1,
                                   std::string_view piece2);

[[nodiscard]] bool StartIgnoreCase(std::string_view text,
                                   std::string_view prefix);

[[nodiscard]] bool EndIgnoreCase(std::string_view text,
                                 std::string_view suffix);

}  // namespace cpp_lib
