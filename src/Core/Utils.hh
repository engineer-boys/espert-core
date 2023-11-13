#ifndef ESPERT_CORE_CORE_HH
#define ESPERT_CORE_CORE_HH

#define ESP_BIND_EVENT_FOR_FUN(obj, ...) std::bind(&obj, this, std::placeholders::_1, ##__VA_ARGS__)

#define PREVENT_COPY(class_name)                     \
  class_name(const class_name&)            = delete; \
  class_name& operator=(const class_name&) = delete;

#ifdef NDEBUG
#define ESP_ASSERT(...) ((void)0);
#else
#define ESP_ASSERT(condition, ...)                                      \
  if (!(condition))                                                     \
  {                                                                     \
    std::cerr << "Assertion failed: " << #condition;                    \
    if (sizeof(__VA_ARGS__) > 1) { std::cerr << " - " << __VA_ARGS__; } \
    std::cerr << "\n";                                                  \
    std::cerr << "in file: " << __FILE__ << "\n";                       \
    std::cerr << "in function: " << __FUNCTION__ << "\n";               \
    std::cerr << "at line: " << __LINE__ << "\n";                       \
    std::abort();                                                       \
  }
#endif

#define ESP_MIN_FRAME_RATE 0.01666666666f // = 1/60s
#define ESP_PI             3.14159265f    // = PI

// std
#include <functional>

namespace esp
{
  // from: https://stackoverflow.com/a/57595105
  template<typename T, typename... Rest> void hashCombine(std::size_t& seed, const T& v, const Rest&... rest)
  {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
  }
} // namespace esp

#endif // ESPERT_CORE_CORE_HH
