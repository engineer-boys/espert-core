#ifndef CORE_RENDER_API_ESP_TEXTURE_FORMAT_HH
#define CORE_RENDER_API_ESP_TEXTURE_FORMAT_HH

#include "esppch.hh"

namespace esp
{
  /*
  Values for this enum  come from Vulkan
  TODO: OpenGL version
  */

  /// @brief Format of texture data. Tells how pixels are aranged.
  enum class EspTextureFormat
  {
    ESP_FORMAT_R8_UNORM            = 9,
    ESP_FORMAT_R8_SRGB             = 15,
    ESP_FORMAT_R8G8_UNORM          = 16,
    ESP_FORMAT_R8G8_SRGB           = 22,
    ESP_FORMAT_R8G8B8_UNORM        = 23,
    ESP_FORMAT_R8G8B8_SRGB         = 29,
    ESP_FORMAT_R8G8B8A8_UNORM      = 37,
    ESP_FORMAT_R8G8B8A8_SRGB       = 43,
    ESP_FORMAT_B8G8R8A8_SRGB       = 50,
    ESP_FORMAT_R16G16B16A16_SFLOAT = 97,
    ESP_FORMAT_R32G32B32A32_SFLOAT = 109,
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_TEXTURE_FORMAT_HH */
