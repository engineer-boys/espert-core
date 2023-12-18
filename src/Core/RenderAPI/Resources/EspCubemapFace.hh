#ifndef RENDER_API_ESP_CUBEMAPFACE_HH
#define RENDER_API_ESP_CUBEMAPFACE_HH

#include "esppch.hh"

namespace esp
{
  enum class EspCubemapFace : uint8_t
  {
    TOP      = 0x01,
    BOTTOM   = 0x02,
    LEFT     = 0x04,
    RIGHT    = 0x08,
    FRONT    = 0x10,
    BACK     = 0x20,
    ENUM_END = 0x40
  };

  inline EspCubemapFace operator++(EspCubemapFace& a)
  {
    if (a == EspCubemapFace::ENUM_END) return a;
    auto copy = a;
    a         = static_cast<EspCubemapFace>(static_cast<uint8_t>(a) << 1);
    return copy;
  }

  inline EspCubemapFace operator++(EspCubemapFace& a, int dummy)
  {
    if (a == EspCubemapFace::ENUM_END) return a;
    a = static_cast<EspCubemapFace>(static_cast<uint8_t>(a) << 1);
    return a;
  }

  inline EspCubemapFace operator--(EspCubemapFace& a)
  {
    if (a == EspCubemapFace::TOP) return a;
    auto copy = a;
    a         = static_cast<EspCubemapFace>(static_cast<uint8_t>(a) >> 1);
    return copy;
  }

  inline EspCubemapFace operator--(EspCubemapFace& a, int dummy)
  {
    if (a == EspCubemapFace::TOP) return a;
    a = static_cast<EspCubemapFace>(static_cast<uint8_t>(a) >> 1);
    return a;
  }

  inline std::string esp_cubemap_face_to_str(EspCubemapFace stage)
  {
    switch (stage)
    {
    case EspCubemapFace::TOP:
      return "top";

    case EspCubemapFace::BOTTOM:
      return "bottom";

    case EspCubemapFace::LEFT:
      return "left";

    case EspCubemapFace::RIGHT:
      return "right";

    case EspCubemapFace::FRONT:
      return "front";

    case EspCubemapFace::BACK:
      return "back";

    default:
      return "invalid";
    }
  }
} // namespace esp

#endif // RENDER_API_ESP_CUBEMAPFACE_HH
