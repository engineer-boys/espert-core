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

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

namespace esp
{
  /// @brief Normalizes object of type T.
  /// @tparam T Type of object to normalize.
  /// @param v Object to normalize.
  /// @return Normalized object.
  template<typename T> T normalize(const T& v)
  {
    if (glm::abs(glm::length(v) - 1.0f) < ESP_EPSILON || glm::abs(glm::length(v)) < ESP_EPSILON) return v;
    return glm::normalize(v);
  }

  /// @brief Combines two hashes.
  /// @tparam T Type of object to hash.
  /// @param seed Previous hashes to combine to.
  /// @param v Object to hash.
  template<class T> inline void hash_combine(std::size_t& seed, const T& v)
  {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  /// @brief Allows running multiple lambdas in std::visit.
  /// @tparam ...Ts Types of lambdas.
  template<class... Ts> struct overloaded : Ts...
  {
    using Ts::operator()...;
  };

  template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
} // namespace esp
#include <bitset>

namespace esp
{
  /// @brief Collection of bits.
  /// @tparam Size size of bitset.
  template<size_t Size> class EspBitset
  {
   private:
    std::bitset<Size> m_bitset;

   public:
    /// @brief Default constructor.
    EspBitset() : m_bitset{ std::bitset<Size>() } {}

    /// @brief Sets range of bits.
    /// @param offset Offset of the range to set.
    /// @param size Size of the range to set.
    inline void set(uint32_t offset, uint32_t size)
    {
      for (uint32_t i = offset; i < size; i++)
      {
        m_bitset.set(i);
      }
    }
    /// @brief Checks if any of the bits in range is set.
    /// @param offset Offset of the range of bits to check.
    /// @param size Size of the range to check.
    /// @return True if any of the bits in range is set. False otherwise.
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

namespace esp
{
  class Event;

  using EventManagerFun = std::function<void(Event&)>;
} // namespace esp

#endif // ESPERT_CORE_CORE_HH
