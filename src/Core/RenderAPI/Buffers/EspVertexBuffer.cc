#include "EspVertexBuffer.hh"

#include "Platform/Vulkan/Buffers/VulkanVertexBuffer.hh"

// using EspVertexBuffer = esp::EspVertexBuffers::EspVertexBuffer;

namespace esp
{
  std::unique_ptr<EspVertexBuffer> EspVertexBuffer::create(void* data, uint32_t vertex_size, uint32_t vertex_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto vertex_buffer = VulkanVertexBuffer::create(data, vertex_size, vertex_count);
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    return vertex_buffer;
  }

  std::unique_ptr<EspVertexBuffers> EspVertexBuffers::create()
  {
    return std::unique_ptr<EspVertexBuffers>(new EspVertexBuffers());
  }

  void EspVertexBuffers::add(void* data, uint32_t vertex_size, uint32_t vertex_count)
  {
    m_vertex_buffers.push_back(EspVertexBuffer::create(data, vertex_size, vertex_count));
  }

  void EspVertexBuffers::attach()
  {
    //...//
  }
} // namespace esp