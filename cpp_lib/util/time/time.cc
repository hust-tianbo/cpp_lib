#include "cpp_lib/util/time/time.h"

#include "absl/time/clock.h"

namespace cpp_lib {

int64_t GetCurrentTimeNanos() { return absl::GetCurrentTimeNanos(); }

int64_t GetCurrentTimeMicros() { return GetCurrentTimeNanos() / 1000L; }

int64_t GetCurrentTimeMillis() { return GetCurrentTimeNanos() / 1000000L; }

int64_t GetCurrentTimeSeconds() { return GetCurrentTimeNanos() / 1000000000L; }

int64_t GetCurrentTimeMinutes() { return GetCurrentTimeSeconds() / 60L; }

int64_t GetCurrentTimeHours() { return GetCurrentTimeMinutes() / 60L; }

int32_t GetIntervalMin(const int32_t& timestamp) {
  int64_t cur_time = GetCurrentTimeSeconds();
  int64_t time_interval = cur_time - timestamp;
  int32_t interval_mins = time_interval / 60;
  return interval_mins;
}

int32_t GetIntervalHour(const int32_t& timestamp) {
  return GetIntervalMin(timestamp) / 60;
}

int32_t GetIntervalDay(const int32_t& timestamp) {
  return GetIntervalHour(timestamp) / 24;
}

// windows实现版本，linux需要换成其他实现方式
int64_t GetCurrentTimeFromField(const std::string& time_field) {
  struct tm tm_temp;
  int year, month, day, hour, minute, second;
  sscanf(time_field.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour,
         &minute, &second);

  tm_temp.tm_year = year - 1990;
  tm_temp.tm_mon = month - 1;
  tm_temp.tm_mday = day, tm_temp.tm_hour = hour;
  tm_temp.tm_min = minute;
  tm_temp.tm_sec = second;
  tm_temp.tm_isdst = 0;

  return mktime(&tm_temp);
}

}  // namespace cpp_lib