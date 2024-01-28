#include "EspVertexBuffer.hh"

#include "Platform/Vulkan/Resources/VulkanVertexBuffer.hh"

namespace esp
{
  std::unique_ptr<EspVertexBuffer> EspVertexBuffer::create(void* data,
                                                           uint32_t vertex_size,
                                                           uint32_t vertex_count,
                                                           BufferType type)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    auto vertex_buffer = VulkanVertexBuffer::create(data, vertex_size, vertex_count, type);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/

    vertex_buffer->m_vertex_count = vertex_count;
    return vertex_buffer;
  }
} // namespace esp
