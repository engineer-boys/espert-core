#ifndef CORE_RENDER_API_ESP_SAMPLE_COUNT_FLAG_HH
#define CORE_RENDER_API_ESP_SAMPLE_COUNT_FLAG_HH

#include "esppch.hh"

namespace esp
{
  /*
  Values for this enum  come from Vulkan
  TODO: OpenGL version
  */

  enum class EspSampleCountFlag
  {
    ESP_SAMPLE_COUNT_1_BIT  = 0x00000001,
    ESP_SAMPLE_COUNT_2_BIT  = 0x00000002,
    ESP_SAMPLE_COUNT_4_BIT  = 0x00000004,
    ESP_SAMPLE_COUNT_8_BIT  = 0x00000008,
    ESP_SAMPLE_COUNT_16_BIT = 0x00000010,
    ESP_SAMPLE_COUNT_32_BIT = 0x00000020,
    ESP_SAMPLE_COUNT_64_BIT = 0x00000040,
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_SAMPLE_COUNT_FLAG_HH */
