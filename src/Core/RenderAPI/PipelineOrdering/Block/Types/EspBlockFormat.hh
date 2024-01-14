#ifndef CORE_RENDER_API_ESP_BLOCK_FORMAT_HH
#define CORE_RENDER_API_ESP_BLOCK_FORMAT_HH

#include "esppch.hh"

namespace esp
{
  /*
  Values for this enum  come from Vulkan
  TODO: OpenGL version
  */

  /// @brief Type of data in a block
  enum class EspBlockFormat
  {
    ESP_FORMAT_R8G8B8A8_UNORM = 37,
    ESP_FORMAT_R8G8B8A8_SRGB  = 43,
    ESP_FORMAT_B8G8R8A8_SRGB  = 50,

    ESP_FORMAT_R16G16_SFLOAT       = 83,
    ESP_FORMAT_R16G16B16A16_SFLOAT = 97,
    ESP_FORMAT_R32G32B32A32_SFLOAT = 109,
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_BLOCK_FORMAT_HH */
