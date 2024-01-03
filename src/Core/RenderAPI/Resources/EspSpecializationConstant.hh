#ifndef RENDER_API_ESP_SPECIALIZATION_CONSTANT_HH
#define RENDER_API_ESP_SPECIALIZATION_CONSTANT_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspShaderStage.hh"

namespace esp
{
  using EspSpecializationValueType = std::variant<bool, float, uint32_t>;

  struct EspSpecializationConstant
  {
    uint32_t constant_id;
    EspSpecializationValueType value;
  };

  inline bool operator==(const EspSpecializationConstant& a, const EspSpecializationConstant& b)
  {
    return a.constant_id == b.constant_id && a.value == b.value;
  }
} // namespace esp

template<> struct std::hash<esp::EspSpecializationConstant>
{
  std::size_t operator()(const esp::EspSpecializationConstant& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.constant_id);
    esp::hash_combine(seed, k.value);
    return seed;
  }
};

template<> struct std::hash<std::vector<esp::EspSpecializationConstant>>
{
  std::size_t operator()(const std::vector<esp::EspSpecializationConstant>& k) const
  {
    std::size_t seed = 0;
    for (const auto& e : k)
    {
      esp::hash_combine(seed, e);
    }
    return seed;
  }
};

using EspSpecializationConstantMap =
    std::unordered_map<esp::EspShaderStage, std::vector<esp::EspSpecializationConstant>>;

template<> struct std::hash<EspSpecializationConstantMap>
{
  std::size_t operator()(const EspSpecializationConstantMap& k) const
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

template<> struct std::hash<std::pair<std::string, EspSpecializationConstantMap>>
{
  std::size_t operator()(const std::pair<std::string, EspSpecializationConstantMap>& k) const
  {
    std::size_t seed = 0;
    esp::hash_combine(seed, k.first);
    esp::hash_combine(seed, k.second);
    return seed;
  }
};

#endif // RENDER_API_ESP_SPECIALIZATION_CONSTANT_HH