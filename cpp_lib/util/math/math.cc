#include "cpp_lib/util/math/math.h"

#include <stdlib.h>
#include <time.h>

namespace cpp_lib {
uint32_t UpperPowerOfTwo(uint32_t v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

int32_t RandomInt32() {
  srand(time(NULL));
  return rand();
}

int32_t RandomInt32(int32_t min, int32_t max) {
  srand(time(NULL));
  int diff = max - min;
  return static_cast<int>((static_cast<double>(diff + 1) / RAND_MAX) * rand() +
                          min);
}

uint32_t Digits10(uint64_t v) {
  int next = v / 10;
  if (next == 0) {
    return 1;
  } else {
    return 1 + Digits10(next);
  }
}

}  // namespace cpp_lib