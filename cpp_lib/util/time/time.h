#pragma once

#include <stdint.h>
#include <time.h>

#include <string>

namespace cpp_lib {
int64_t GetCurrentTimeNanos();

int64_t GetCurrentTimeMicros();

int64_t GetCurrentTimeMillis();

int64_t GetCurrentTimeSeconds();

int64_t GetCurrentTimeMinutes();

int64_t GetCurrentTimeHours();

int32_t GetIntervalDay(const int32_t& timestamp);

int32_t GetIntervalHour(const int32_t& timestamp);

int32_t GetIntervalMin(const int32_t& timestamp);

int64_t GetCurrentTimeFromField(const std::string& time_field);
}  // namespace cpp_lib