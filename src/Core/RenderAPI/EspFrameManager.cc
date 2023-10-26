#include "EspFrameManager.hh"

// #include "Platform/OpenGL/OpenGLFrameScheduler.hh"
#include "Platform/Vulkan/VulkanFrameManager.hh"

namespace esp
{
  bool EspFrameManager::s_is_exist = false;

  std::unique_ptr<EspFrameManager> EspFrameManager::create_and_init(EspWindow& window, glm::vec4 clear_color)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto frame_scheduler = VulkanFrameManager::create();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    frame_scheduler->m_window_width  = window.get_width();
    frame_scheduler->m_window_height = window.get_height();
    frame_scheduler->set_clear_color(clear_color);
    frame_scheduler->init();
    return frame_scheduler;
  }

  void EspFrameManager::on_window_resized(WindowResizedEvent& e)
  {
    m_window_width  = e.get_width();
    m_window_height = e.get_height();
  }

  std::pair<uint32_t, uint32_t> EspFrameManager::get_swap_chain_extent() { return { m_window_width, m_window_height }; }

  void EspFrameManager::set_depth_stencil(float depth, uint32_t stencil)
  {
    m_depth_stencil.m_depth   = depth;
    m_depth_stencil.m_stencil = stencil;
  }

  void EspFrameManager::set_clear_color(glm::vec3 color) { m_clear_color = glm::vec4(color, 1.f); }

  void EspFrameManager::set_clear_color(glm::vec4 color) { m_clear_color = color; }
} // namespace esp