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
#if ESP_USE_VULKAN
    auto context = VulkanContext::create(window);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/

    return context;
  }

} // namespace esp