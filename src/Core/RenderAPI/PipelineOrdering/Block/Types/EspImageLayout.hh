#ifndef CORE_RENDER_API_ESP_IMAGE_LAYOUT_HH
#define CORE_RENDER_API_ESP_IMAGE_LAYOUT_HH

#include "esppch.hh"

namespace esp
{
  /*
  Values for this enum  come from Vulkan
  TODO: OpenGL version
  */

  enum class EspImageLayout
  {
    ESP_IMAGE_LAYOUT_UNDEFINED                = 0,
    ESP_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
    ESP_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
    ESP_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL     = 6,
    ESP_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL     = 7,
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_IMAGE_LAYOUT_HH */