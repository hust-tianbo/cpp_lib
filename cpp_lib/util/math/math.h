#pragma once

#include <stdint.h>

namespace cpp_lib {
// 生成最接近输入的最大的2指数次幂
uint32_t UpperPowerOfTwo(uint32_t v);

// 生成32位的随机数
int32_t RandomInt32();

// 生成[min,max]之间的32位随机数
int32_t RandomInt32(int32_t min, int32_t max);

// 计算10进制数的位数
uint32_t Digits10(uint64_t v);
}  // namespace cpp_lib