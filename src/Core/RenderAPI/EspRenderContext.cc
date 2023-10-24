#include "EspRenderContext.hh"

// #include "Platform/OpenGL/OpenGLContext.hh"
#include "Platform/Vulkan/VulkanContext.hh"

namespace esp
{
  bool EspRenderContext::s_is_exist = false;

  std::unique_ptr<EspRenderContext> EspRenderContext::create_and_init(EspWindow& window)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto context = VulkanContext::create();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    context->init(window);
    return context;
  }

} // namespace esp