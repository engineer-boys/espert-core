#include "EspDepthBlock.hh"

#include "Core/RenderAPI/Work/EspWorkOrchestrator.hh"
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanDepthBlock.hh"

namespace esp
{
  EspDepthBlock::EspDepthBlock(EspDepthBlockFormat format, uint32_t width, uint32_t height) :
      m_format{ format }, m_width{ width }, m_height{ height }
  {
  }

  std::unique_ptr<EspDepthBlock> EspDepthBlock::build(EspDepthBlockFormat format)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
    //     // #if defined(OPENGL_PLATFORM)
    //     // #elif defined(VULKAN_PLATFORM)
    auto [width, height] = EspWorkOrchestrator::get_swap_chain_extent();
    return std::make_unique<VulkanDepthBlock>(format, width, height);
    //     // #else
    //     // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    //     // #endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp
