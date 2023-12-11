#include "Core/RenderAPI/EspDebugMessenger.hh"
#include "Platform/Vulkan/VulkanDebugMessenger.hh"

namespace esp
{
  EspDebugMessenger* EspDebugMessenger::s_instance = nullptr;
  EspDebugMessenger::EspDebugMessenger() : m_error_count(0), m_warning_count(0), m_info_count(0)
  {
    if (EspDebugMessenger::s_instance != nullptr)
    {
      throw std::runtime_error("The debug messenger instance already exists!");
    }

    EspDebugMessenger::s_instance = this;
  }

  std::unique_ptr<EspDebugMessenger> EspDebugMessenger::create()
  {
    auto debug_messenger = VulkanDebugMessenger::create();

    return debug_messenger;
  }
} // namespace esp
