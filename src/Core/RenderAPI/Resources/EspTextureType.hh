#ifndef RENDER_API_ESP_TEXTURE_TYPE_HH
#define RENDER_API_ESP_TEXTURE_TYPE_HH

#include "esppch.hh"

namespace esp
{
  using EspTextureTypeFlags = uint8_t;
  enum class EspTextureType : uint8_t
  {
    ALBEDO    = 0x01,
    NORMAL    = 0x02,
    METALLIC  = 0x04,
    ROUGHNESS = 0x08,
    AO        = 0x10,
    ENUM_END  = 0x20
  };

  inline EspTextureTypeFlags operator|(const EspTextureType a, const EspTextureType b)
  {
    return static_cast<EspTextureTypeFlags>(a) | static_cast<EspTextureTypeFlags>(b);
  }

  inline EspTextureTypeFlags operator|(const EspTextureTypeFlags a, const EspTextureType b)
  {
    return a | static_cast<EspTextureTypeFlags>(b);
  }

  inline EspTextureTypeFlags operator&(const EspTextureTypeFlags a, const EspTextureType b)
  {
    return a & static_cast<EspTextureTypeFlags>(b);
  }

  inline EspTextureType operator++(EspTextureType& a)
  {
    if (a == EspTextureType::ENUM_END) return a;
    auto copy = a;
    a         = static_cast<EspTextureType>(static_cast<uint8_t>(a) << 1);
    return copy;
  }

  inline EspTextureType operator++(EspTextureType& a, int dummy)
  {
    if (a == EspTextureType::ENUM_END) return a;
    a = static_cast<EspTextureType>(static_cast<uint8_t>(a) << 1);
    return a;
  }

  inline EspTextureType operator--(EspTextureType& a)
  {
    if (a == EspTextureType::ALBEDO) return a;
    auto copy = a;
    a         = static_cast<EspTextureType>(static_cast<uint8_t>(a) >> 1);
    return copy;
  }

  inline EspTextureType operator--(EspTextureType& a, int dummy)
  {
    if (a == EspTextureType::ALBEDO) return a;
    a = static_cast<EspTextureType>(static_cast<uint8_t>(a) >> 1);
    return a;
  }

  inline aiTextureType esp_texture_type_to_assimp(EspTextureType type)
  {
    switch (type)
    {
    case EspTextureType::ALBEDO:
      return aiTextureType_DIFFUSE;

    case EspTextureType::NORMAL:
      return aiTextureType_NORMALS;

    case EspTextureType::METALLIC:
      return aiTextureType_METALNESS;

    case EspTextureType::ROUGHNESS:
      return aiTextureType_DIFFUSE_ROUGHNESS;

    case EspTextureType::AO:
      return aiTextureType_AMBIENT_OCCLUSION;

    default:
      return aiTextureType_NONE;
    }
  }
} // namespace esp

#endif // RENDER_API_ESP_TEXTURE_TYPE_HH