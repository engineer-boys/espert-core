#include "MiniRContext.hh"

#include "Platforms/OpenGLMiniRAPI/OpenGLMiniRAPIContext.hh"
#include "Platforms/VulkanMiniRAPI/VulkanMiniRAPIContext.hh"

namespace esp
{
  bool MiniRContext::s_is_exist = false;

  std::unique_ptr<MiniRContext> MiniRContext::create_and_init(EspWindow& window)
  {
/* ---------------------------------------------------------*/
/* ------------- PLATFORM DEPENDENT ------------------------*/
/* ---------------------------------------------------------*/
#if defined(OPENGL_PLATFORM)
    auto context = std::make_unique<OpenGLMiniRAPIContext>();
#elif defined(VULKAN_PLATFORM)
    auto context = VulkanMiniRAPIContext::create();
#else
#error Unfortunatelly, neither Vulkan nor OpenGL is supported.
#endif
    /* ---------------------------------------------------------*/

    context->init(window);
    return context;
  }

} // namespace esp
