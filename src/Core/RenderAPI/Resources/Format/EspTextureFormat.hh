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
    ESP_FORMAT_R8G8B8A8_UNORM = 37,
    ESP_FORMAT_B8G8R8A8_SRGB  = 50
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_TEXTURE_FORMAT_HH */