#ifndef RENDER_API_ESP_SPECIALIZATION_CONSTANT_HH
#define RENDER_API_ESP_SPECIALIZATION_CONSTANT_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspShaderStage.hh"

namespace esp
{
  using EspSpecializationValueType = std::variant<bool, float, uint32_t>;

  /// @brief Describes single specialization constant.
  struct EspSpecificationConstant
  {
    /// @brief Id of specialization constant.
    uint32_t constant_id;
    /// @brief Value of specialization constant.
    EspSpecializationValueType value;
  };

  /// @brief Equals operator checks whether two EspSpecializationConstant objects have the same value.
  /// @param a First EspSpecificationConstant.
  /// @param b Second EspSpecificationConstant.
  /// @return True if values are the same. False otherwise.
  inline bool operator==(const EspSpecificationConstant& a, const EspSpecificationConstant& b)
  {
    return a.constant_id == b.constant_id && a.value == b.value;
  }
} // namespace esp

/// @brief Hashes EspSpecificationConstant.
template<> struct std::hash<esp::EspSpecificationConstant>
{
  /// @brief Hashes EspSpecificationConstant.
  /// @param k EspSpecificationConstant to be hashed.
  /// @return EspSpecificationConstant hash.
  std::size_t operator()(const esp::EspSpecificationConstant& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.constant_id);
    esp::hash_combine(seed, k.value);
    return seed;
  }
};

/// @brief Hashes vector of EspSpecificationConstant.
template<> struct std::hash<std::vector<esp::EspSpecificationConstant>>
{
  /// @brief Hashes vector of EspSpecificationConstant.
  /// @param k Vector of EspSpecificationConstant to be hashed.
  /// @return EspSpecificationConstant vector hash.
  std::size_t operator()(const std::vector<esp::EspSpecificationConstant>& k) const
  {
    std::size_t seed = 0;
    for (const auto& e : k)
    {
      esp::hash_combine(seed, e);
    }
    return seed;
  }
};

using SpecializationConstantMap = std::unordered_map<esp::EspShaderStage, std::vector<esp::EspSpecificationConstant>>;

/// @brief Hashes SpecializationConstantMap.
template<> struct std::hash<SpecializationConstantMap>
{
  /// @brief Hashes SpecializationConstantMap.
  /// @param k SpecializationConstantMap to be hashed.
  /// @return SpecializationConstantMap hash.
  std::size_t operator()(const SpecializationConstantMap& k) const
  {
    std::size_t seed = 0;
    for (const auto& e : k)
    {
      esp::hash_combine(seed, e.first);
      esp::hash_combine(seed, e.second);
    }
    return seed;
  }
};

/// @brief Hashes pair of string and SpecializationConstantMap.
template<> struct std::hash<std::pair<std::string, SpecializationConstantMap>>
{
  /// @brief Hashes pair of string and SpecializationConstantMap.
  /// @param k Pair of string and SpecializationConstantMap to be hashed.
  /// @return Pair of string and SpecializationConstantMap hash.
  std::size_t operator()(const std::pair<std::string, SpecializationConstantMap>& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.first);
    esp::hash_combine(seed, k.second);
    return seed;
  }
};

#endif // RENDER_API_ESP_SPECIALIZATION_CONSTANT_HH