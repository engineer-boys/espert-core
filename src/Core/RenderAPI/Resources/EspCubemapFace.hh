#ifndef RENDER_API_ESP_CUBEMAPFACE_HH
#define RENDER_API_ESP_CUBEMAPFACE_HH

#include "esppch.hh"

namespace esp
{
  /// @brief Type of face in cubemap.
  enum class EspCubemapFace : uint8_t
  {
    RIGHT    = 0x01,
    LEFT     = 0x02,
    TOP      = 0x04,
    BOTTOM   = 0x08,
    FRONT    = 0x10,
    BACK     = 0x20,
    ENUM_END = 0x40
  };

  /// @brief Sets cubemap face to next face type. (useful for iterating enum)
  /// @param a Reference to cubemap face.
  /// @return Cubemap face before 'incrementation'.
  inline EspCubemapFace operator++(EspCubemapFace& a)
  {
    if (a == EspCubemapFace::ENUM_END) return a;
    auto copy = a;
    a         = static_cast<EspCubemapFace>(static_cast<uint8_t>(a) << 1);
    return copy;
  }

  /// @brief Sets cubemap face to next face type. (useful for iterating enum)
  /// @param a Reference to cubemap face.
  /// @param dummy Dummy parameter needed to differentiate incrementation operators.
  /// @return Cubemap face after 'incrementation'.
  inline EspCubemapFace operator++(EspCubemapFace& a, int dummy)
  {
    if (a == EspCubemapFace::ENUM_END) return a;
    a = static_cast<EspCubemapFace>(static_cast<uint8_t>(a) << 1);
    return a;
  }

  /// @brief Sets cubemap face to previous face type. (useful for iterating enum)
  /// @param a Reference to cubemap face.
  /// @return Cubemap face before 'decrementation'.
  inline EspCubemapFace operator--(EspCubemapFace& a)
  {
    if (a == EspCubemapFace::RIGHT) return a;
    auto copy = a;
    a         = static_cast<EspCubemapFace>(static_cast<uint8_t>(a) >> 1);
    return copy;
  }

  /// @brief Sets cubemap face to previous face type. (useful for iterating enum)
  /// @param a Reference to cubemap face.
  /// @param dummy Dummy parameter needed to differentiate decrementation operators.
  /// @return Cubemap face before 'decrementation'.
  inline EspCubemapFace operator--(EspCubemapFace& a, int dummy)
  {
    if (a == EspCubemapFace::RIGHT) return a;
    a = static_cast<EspCubemapFace>(static_cast<uint8_t>(a) >> 1);
    return a;
  }

  /// @brief Converts cubemap face to human readable string.
  /// @param face Cubemap face. 
  /// @return Human readable string describing cubemap face.
  inline std::string esp_cubemap_face_to_str(EspCubemapFace face)
  {
    switch (face)
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
