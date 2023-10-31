#include "EspUniformMetaData.hh"

#include "Platform/Vulkan/VulkanUniformMetaData.hh"

namespace esp
{
  std::unique_ptr<EspUniformMetaData> EspUniformMetaData::create()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    // #elif defined(VULKAN_PLATFORM)
    return std::make_unique<VulkanUniformMetaData>();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
  }
} // namespace esp
