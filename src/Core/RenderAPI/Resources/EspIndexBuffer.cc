#include "EspIndexBuffer.hh"

#include "Platform/Vulkan/Resources/VulkanIndexBuffer.hh"

namespace esp
{
  std::unique_ptr<EspIndexBuffer> EspIndexBuffer::create(uint32_t* indices, uint32_t index_count)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto index_buffer = VulkanIndexBuffer::create(indices, index_count);
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    index_buffer->m_index_count = index_count;
    return index_buffer;
  }
} // namespace esp