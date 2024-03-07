#include "EspGui.hh"
#include "Platform/Vulkan/VulkanGui.hh"

namespace esp
{
  bool EspGui::m_use_gui = false;

  std::unique_ptr<EspGui> EspGui::create(const std::string& name)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    auto gui = VulkanGui::create();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/

    gui->m_name = name;

    return gui;
  }

  void EspGui::terminate()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanGui::terminate();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspGui::new_frame()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanGui::new_frame();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspGui::begin()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanGui::begin();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspGui::end()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanGui::end();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspGui::end_frame()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanGui::end_frame();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }

  void EspGui::render()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanGui::render();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }
} // namespace esp