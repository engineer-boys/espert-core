#include "EspRenderContext.hh"

namespace esp
{
  EspRenderContext* EspRenderContext::s_instance = nullptr;

  std::unique_ptr<EspRenderContext>
  EspRenderContext::create_and_init_vulkan(esp::EspWindow& window)
  {
    if (EspRenderContext::s_instance != nullptr)
    {
      ESP_CORE_ERROR("Render context already exists");
      throw std::runtime_error("Render context already exists");
    }

    return std::unique_ptr<EspRenderContext>(new EspRenderContext(window));
  }

  EspRenderContext::~EspRenderContext() { s_instance = nullptr; }

  EspRenderContext::EspRenderContext(esp::EspWindow& window) :
      m_device{ window }, m_scheduler{ window, m_device }
  {
    EspRenderContext::s_instance = this;
  }
} // namespace esp