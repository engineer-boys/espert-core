#ifndef CORE_RENDER_API_RESOURCES_ESP_IMAGE_SUBRESOURCE_RANGE_HH
#define CORE_RENDER_API_RESOURCES_ESP_IMAGE_SUBRESOURCE_RANGE_HH

#include "esppch.hh"

namespace esp
{
  class EspImageSubresourceRange
  {
   public:
    uint32_t m_base_mip_level;
    uint32_t m_level_count;

    uint32_t m_base_array_layer;
    uint32_t m_layer_count;
  };
} // namespace esp

#endif // CORE_RENDER_API_RESOURCES_ESP_IMAGE_SUBRESOURCE_RANGE_HH
