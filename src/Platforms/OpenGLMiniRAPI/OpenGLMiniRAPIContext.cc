#include "OpenGLMiniRAPIContext.hh"

namespace esp
{
  OpenGLMiniRAPIContext::OpenGLMiniRAPIContext() {}
  OpenGLMiniRAPIContext::~OpenGLMiniRAPIContext() {}

  void OpenGLMiniRAPIContext::init(EspWindow& window) {}
  void OpenGLMiniRAPIContext::terminate() {}
  void OpenGLMiniRAPIContext::update() {}

  void MiniR_context_glfw_hints() { ESP_CORE_INFO("GLFW hints for openGL"); }
} // namespace esp