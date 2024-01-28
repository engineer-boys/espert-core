#include "EspUniformMetaData.hh"

#include "Platform/Vulkan/Uniforms/VulkanUniformMetaData.hh"

namespace esp
{
  std::unique_ptr<EspUniformMetaData> EspUniformMetaData::create()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    return std::make_unique<VulkanUniformMetaData>();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/
  }
} // namespace esp
