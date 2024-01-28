#ifndef CORE_RENDER_API_ESP_COMPARE_OP_HH
#define CORE_RENDER_API_ESP_COMPARE_OP_HH

#include "esppch.hh"

namespace esp
{
  /*
  Values for this enum  come from Vulkan
  TODO: OpenGL version
  */

  /// @brief Compare operator for depth test.
  enum class EspCompareOp
  {
    ESP_COMPARE_OP_NEVER            = 0,
    ESP_COMPARE_OP_LESS             = 1,
    ESP_COMPARE_OP_EQUAL            = 2,
    ESP_COMPARE_OP_LESS_OR_EQUAL    = 3,
    ESP_COMPARE_OP_GREATER          = 4,
    ESP_COMPARE_OP_NOT_EQUAL        = 5,
    ESP_COMPARE_OP_GREATER_OR_EQUAL = 6,
    ESP_COMPARE_OP_ALWAYS           = 7,
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_COMPARE_OP_HH */
