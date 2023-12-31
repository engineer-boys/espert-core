#ifndef CORE_RENDER_API_ESP_TEXTURE_FORMAT_HH
#define CORE_RENDER_API_ESP_TEXTURE_FORMAT_HH

#include "esppch.hh"

namespace esp
{
  /*
  Values for this enum  come from Vulkan
  TODO: OpenGL version
  */

  enum class EspTextureFormat
  {
    ESP_FORMAT_R8_UNORM       = 9,
    ESP_FORMAT_R8_SRGB        = 15,
    ESP_FORMAT_R8G8_UNORM     = 16,
    ESP_FORMAT_R8G8_SRGB      = 22,
    ESP_FORMAT_R8G8B8_UNORM   = 23,
    ESP_FORMAT_R8G8B8_SRGB    = 29,
    ESP_FORMAT_R8G8B8A8_UNORM = 37,
    ESP_FORMAT_R8G8B8A8_SRGB  = 43
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_TEXTURE_FORMAT_HH */