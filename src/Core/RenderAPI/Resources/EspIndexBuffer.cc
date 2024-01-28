#include "EspIndexBuffer.hh"

#include "Platform/Vulkan/Resources/VulkanIndexBuffer.hh"

namespace esp
{
  std::unique_ptr<EspIndexBuffer> EspIndexBuffer::create(uint32_t* indices, uint32_t index_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    auto index_buffer = VulkanIndexBuffer::create(indices, index_count);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/

    index_buffer->m_index_count = index_count;
    return index_buffer;
  }
} // namespace esp
