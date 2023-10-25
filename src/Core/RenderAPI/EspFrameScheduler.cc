#include "EspFrameScheduler.hh"

// #include "Platform/OpenGL/OpenGLFrameScheduler.hh"
#include "Platform/Vulkan/VulkanFrameScheduler.hh"

namespace esp
{
  bool EspFrameScheduler::s_is_exist = false;

  std::unique_ptr<EspFrameScheduler> EspFrameScheduler::create_and_init(EspWindow& window)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto frame_scheduler = VulkanFrameScheduler::create();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    frame_scheduler->init(window);
    return frame_scheduler;
  }
} // namespace esp