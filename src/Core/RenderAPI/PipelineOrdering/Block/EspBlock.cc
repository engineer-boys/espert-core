#include "EspBlock.hh"

#include "Core/RenderAPI/Work/EspWorkOrchestrator.hh"
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanBlock.hh"

namespace esp
{
  EspBlock::EspBlock(EspBlockFormat format,
                     EspSampleCountFlag sample_count_flag,
                     uint32_t width,
                     uint32_t height,
                     glm::vec3 clear_color) :
      m_format{ format },
      m_sample_count_flag{ sample_count_flag }, m_width{ width }, m_height{ height }, m_clear_color{ clear_color }
  {
  }

  std::shared_ptr<EspBlock> EspBlock::build(EspBlockFormat format,
                                            EspSampleCountFlag sample_count_flag,
                                            glm::vec3 clear_color)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
    //     // #if defined(OPENGL_PLATFORM)
    //     // #elif defined(VULKAN_PLATFORM)
    auto [width, height] = EspWorkOrchestrator::get_swap_chain_extent();
    return std::make_shared<VulkanBlock>(format, sample_count_flag, width, height, clear_color);
    //     // #else
    //     // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    //     // #endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp
