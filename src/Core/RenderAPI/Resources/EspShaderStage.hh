#ifndef RENDER_API_ESP_SHADER_STAGE_HH
#define RENDER_API_ESP_SHADER_STAGE_HH

#include "esppch.hh"

namespace esp
{
  using EspShaderStageFlags = uint8_t;

  /// @brief Represent type of shader stage
  enum class EspShaderStage : uint8_t
  {
    VERTEX                 = 0x01,
    TESSELATION_CONTROL    = 0x02,
    TESSELATION_EVALUATION = 0x04,
    GEOMETRY               = 0x08,
    FRAGMENT               = 0x10,
    COMPUTE                = 0x20,
    ENUM_END               = 0x40
  };

  /// @brief Connects two EspShaderStages to form EspShaderStageFlags.
  /// @param a First EspShaderStage.
  /// @param b Second EspShaderStage.
  /// @return EspShaderFlags with bits set for both stages.
  inline EspShaderStageFlags operator|(const EspShaderStage a, const EspShaderStage b)
  {
    return static_cast<EspShaderStageFlags>(a) | static_cast<EspShaderStageFlags>(b);
  }

  /// @brief Adds EspShaderStage to EspShaderStageFlags.
  /// @param a EspShaderStageFlags to set the stage on.
  /// @param b EspShaderStageFlag to set in EspShaderStageFlags.
  /// @return EspShaderFlags with new stage set.
  inline EspShaderStageFlags operator|(const EspShaderStageFlags a, const EspShaderStage b)
  {
    return a | static_cast<EspShaderStageFlags>(b);
  }

  /// @brief Unsets EspShaderStage from EspShaderStageFlags.
  /// @param a EspShaderStageFlags to unset EspShaderStage from.
  /// @param b EspShaderStage that will be unset in EspShaderStageFlags.
  /// @return EspShaderStageFlags with stage unset.
  inline EspShaderStageFlags operator&(const EspShaderStageFlags a, const EspShaderStage b)
  {
    return a & static_cast<EspShaderStageFlags>(b);
  }

  /// @brief Sets shader stage to next stage type. (useful for iterating enum)
  /// @param a Reference to shader stage.
  /// @return Shader stage before 'incrementation'.
  inline EspShaderStage operator++(EspShaderStage& a)
  {
    if (a == EspShaderStage::ENUM_END) return a;
    auto copy = a;
    a         = static_cast<EspShaderStage>(static_cast<uint8_t>(a) << 1);
    return copy;
  }

  /// @brief Sets shader stage to next stage type. (useful for iterating enum)
  /// @param a Reference to shader stage.
  /// @param dummy Dummy parameter needed to differentiate incrementation operators.
  /// @return Shader stage after 'incrementation'.
  inline EspShaderStage operator++(EspShaderStage& a, int dummy)
  {
    if (a == EspShaderStage::ENUM_END) return a;
    a = static_cast<EspShaderStage>(static_cast<uint8_t>(a) << 1);
    return a;
  }

  /// @brief Sets shader stage to previous stage type. (useful for iterating enum)
  /// @param a Reference to shader stage.
  /// @return Shader stage before 'decrementation'.
  inline EspShaderStage operator--(EspShaderStage& a)
  {
    if (a == EspShaderStage::VERTEX) return a;
    auto copy = a;
    a         = static_cast<EspShaderStage>(static_cast<uint8_t>(a) >> 1);
    return copy;
  }

  /// @brief Sets shader stage to previous stage type. (useful for iterating enum)
  /// @param a Reference to shader stage.
  /// @param dummy Dummy parameter needed to differentiate decrementation operators.
  /// @return Shader stage before 'decrementation'.
  inline EspShaderStage operator--(EspShaderStage& a, int dummy)
  {
    if (a == EspShaderStage::VERTEX) return a;
    a = static_cast<EspShaderStage>(static_cast<uint8_t>(a) >> 1);
    return a;
  }

  /// @brief Converts shader stage to human readable string.
  /// @param face Shader stage.
  /// @return Human readable string describing shader stage.
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
