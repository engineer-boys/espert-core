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
#define ESP_EPSILON        0.0001f

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

namespace esp
{
  template<typename T> T normalize(const T& v)
  {
    if (glm::abs(glm::length(v) - 1.0f) < ESP_EPSILON || glm::abs(glm::length(v)) < ESP_EPSILON) return v;
    return glm::normalize(v);
  }
} // namespace esp
#include <bitset>

namespace esp
{
  template<size_t Size> class EspBitset
  {
   private:
    std::bitset<Size> m_bitset;

   public:
    EspBitset() : m_bitset{ std::bitset<Size>() } {}

    inline void set(uint32_t offset, uint32_t size)
    {
      for (uint32_t i = offset; i < size; i++)
      {
        m_bitset.set(i);
      }
    }
    inline bool any(uint32_t offset, uint32_t size)
    {
      for (uint32_t i = offset; i < size; i++)
      {
        if (m_bitset.test(i)) return true;
      }
      return false;
    }
  };
} // namespace esp

#endif // ESPERT_CORE_CORE_HH
