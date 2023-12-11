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
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto vertex_buffer = VulkanVertexBuffer::create(data, vertex_size, vertex_count, type);
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    vertex_buffer->m_vertex_count = vertex_count;
    return vertex_buffer;
  }
} // namespace esp