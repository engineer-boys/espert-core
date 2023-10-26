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

    frame_scheduler->m_window_width  = window.get_width();
    frame_scheduler->m_window_height = window.get_height();
    frame_scheduler->init();
    return frame_scheduler;
  }

  void EspFrameScheduler::on_window_resized(WindowResizedEvent& e)
  {
    m_window_width  = e.get_width();
    m_window_height = e.get_height();
  }

  void EspFrameScheduler::set_depth_stencil(float depth, uint32_t stencil)
  {
    m_depth_stencil.m_depth   = depth;
    m_depth_stencil.m_stencil = stencil;
  }

  void EspFrameScheduler::set_clear_color(glm::vec3 color) { m_clear_color = glm::vec4(color, 1.f); }

  void EspFrameScheduler::set_clear_color(glm::vec4 color) { m_clear_color = color; }
} // namespace esp