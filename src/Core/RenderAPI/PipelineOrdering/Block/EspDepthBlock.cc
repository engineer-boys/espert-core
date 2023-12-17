#include "EspDepthBlock.hh"

#include "Platform/Vulkan/PipelineOrdering/Block/VulkanDepthBlock.hh"

namespace esp
{
  EspDepthBlock::EspDepthBlock(uint32_t width, uint32_t height, glm::vec3 clear_color) :
      m_width{ width }, m_height{ height }, m_clear_color{ clear_color }
  {
  }

  std::unique_ptr<EspDepthBlock> EspDepthBlock::build(uint32_t width, uint32_t height, glm::vec3 clear_color)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
    //     // #if defined(OPENGL_PLATFORM)
    //     // #elif defined(VULKAN_PLATFORM)
    return std::make_unique<VulkanDepthBlock>(width, height, clear_color);
    //     // #else
    //     // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    //     // #endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp
