#ifndef RENDER_API_ESP_SHADER_STAGE_HH
#define RENDER_API_ESP_SHADER_STAGE_HH

#include "esppch.hh"

namespace esp
{
  using EspShaderStageFlags = uint8_t;

  enum class EspShaderStage : uint8_t
  {
    VERTEX                 = 0x01,
    TESSELATION_CONTROL    = 0x02,
    TESSELATION_EVALUATION = 0x04,
    GEOMETRY               = 0x08,
    FRAGMENT               = 0x10,
    COMPUTE                = 0x20,
    ENUM_END               = 0x40,
    ALL                    = 0x3f
  };

  inline EspShaderStageFlags operator|(const EspShaderStage a, const EspShaderStage b)
  {
    return static_cast<EspShaderStageFlags>(a) | static_cast<EspShaderStageFlags>(b);
  }

  inline EspShaderStageFlags operator|(const EspShaderStageFlags a, const EspShaderStage b)
  {
    return a | static_cast<EspShaderStageFlags>(b);
  }

  inline EspShaderStageFlags operator|=(EspShaderStageFlags& a, const EspShaderStage b)
  {
    return a = a | static_cast<EspShaderStageFlags>(b);
  }

  inline EspShaderStageFlags operator&(const EspShaderStageFlags a, const EspShaderStage b)
  {
    return a & static_cast<EspShaderStageFlags>(b);
  }

  inline EspShaderStageFlags operator&=(EspShaderStageFlags& a, const EspShaderStage b)
  {
    return a = a & static_cast<EspShaderStageFlags>(b);
  }

  inline EspShaderStage operator++(EspShaderStage& a)
  {
    if (a == EspShaderStage::ENUM_END) return a;
    auto copy = a;
    a         = static_cast<EspShaderStage>(static_cast<uint8_t>(a) << 1);
    return copy;
  }

  inline EspShaderStage operator++(EspShaderStage& a, int dummy)
  {
    if (a == EspShaderStage::ENUM_END) return a;
    a = static_cast<EspShaderStage>(static_cast<uint8_t>(a) << 1);
    return a;
  }

  inline EspShaderStage operator--(EspShaderStage& a)
  {
    if (a == EspShaderStage::VERTEX) return a;
    auto copy = a;
    a         = static_cast<EspShaderStage>(static_cast<uint8_t>(a) >> 1);
    return copy;
  }

  inline EspShaderStage operator--(EspShaderStage& a, int dummy)
  {
    if (a == EspShaderStage::VERTEX) return a;
    a = static_cast<EspShaderStage>(static_cast<uint8_t>(a) >> 1);
    return a;
  }

  inline std::string esp_shader_stage_to_string(EspShaderStage stage)
  {
    switch (stage)
    {
    case EspShaderStage::VERTEX:
      return "vertex";

    case EspShaderStage::TESSELATION_CONTROL:
      return "tesselation control";

    case EspShaderStage::TESSELATION_EVALUATION:
      return "tesselation evaluation";

    case EspShaderStage::GEOMETRY:
      return "geometry";

    case EspShaderStage::FRAGMENT:
      return "fragment";

    case EspShaderStage::COMPUTE:
      return "compute";

    default:
      return "invalid";
    }
  }
} // namespace esp

#endif // RENDER_API_ESP_SHADER_STAGE_HH