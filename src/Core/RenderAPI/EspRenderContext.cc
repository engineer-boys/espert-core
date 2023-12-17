#include "EspRenderContext.hh"

// #include "Platform/OpenGL/OpenGLContext.hh"
#include "Platform/Vulkan/VulkanContext.hh"

namespace esp
{
  std::unique_ptr<EspRenderContext> EspRenderContext::build(EspWindow& window)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto context = VulkanContext::create(window);
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    return context;
  }

} // namespace esp