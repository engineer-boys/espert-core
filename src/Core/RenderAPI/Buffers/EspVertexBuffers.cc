#include "EspVertexBuffers.hh"

#include "Platform/Vulkan/Buffers/VulkanVertexBuffers.hh"

namespace esp
{
  std::unique_ptr<EspVertexBuffers> EspVertexBuffers::create()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto vertex_buffers = VulkanVertexBuffers::create();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    return vertex_buffers;
  }
} // namespace esp