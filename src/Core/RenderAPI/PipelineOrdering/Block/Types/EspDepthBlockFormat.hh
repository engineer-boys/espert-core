#ifndef CORE_RENDER_API_ESP_DEPTH_BLOCK_FORMAT_HH
#define CORE_RENDER_API_ESP_DEPTH_BLOCK_FORMAT_HH

#include "esppch.hh"

namespace esp
{
  /*
  Values for this enum  come from Vulkan
  TODO: OpenGL version
  */

  /// @brief Type of data in a depth block
  enum class EspDepthBlockFormat
  {
    ESP_FORMAT_D32_SFLOAT         = 126,
    ESP_FORMAT_D24_UNORM_S8_UINT  = 129,
    ESP_FORMAT_D32_SFLOAT_S8_UINT = 130,
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_DEPTH_BLOCK_FORMAT_HH */
