#ifndef CORE_RENDER_API_RESOURCES_ESP_IMAGE_COPY_HH
#define CORE_RENDER_API_RESOURCES_ESP_IMAGE_COPY_HH

#include "esppch.hh"

namespace esp
{
  class EspImageSubresourceLayers
  {
   public:
    uint32_t m_mip_level;
    uint32_t m_base_array_layer;
    uint32_t m_layer_count;
  };

  class EspImageCopy
  {
   public:
    EspImageSubresourceLayers src_subresource;
    EspImageSubresourceLayers dst_subresource;

    uint32_t m_width;
    uint32_t m_height;
  };
} // namespace esp

#endif // CORE_RENDER_API_RESOURCES_ESP_IMAGE_COPY_HH
