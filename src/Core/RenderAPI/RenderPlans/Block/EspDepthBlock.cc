#include "EspDepthBlock.hh"

#include "Core/RenderAPI/Work/EspWorkOrchestrator.hh"
#include "Platform/Vulkan/RenderPlans/Block/VulkanDepthBlock.hh"

namespace esp
{
  EspDepthBlock::EspDepthBlock(EspDepthBlockFormat format,
                               EspSampleCountFlag sample_count_flag,
                               EspImageUsageFlag image_usage_flag,
                               uint32_t width,
                               uint32_t height) :
      m_format{ format },
      m_sample_count_flag{ sample_count_flag }, m_image_usage_flag{ image_usage_flag }, m_width{ width },
      m_height{ height }
  {
  }

  std::unique_ptr<EspDepthBlock> EspDepthBlock::build(EspDepthBlockFormat format,
                                                      EspSampleCountFlag sample_count_flag,
                                                      EspImageUsageFlag image_usage_flag)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    auto [width, height] = EspWorkOrchestrator::get_swap_chain_extent();
    return std::make_unique<VulkanDepthBlock>(format, sample_count_flag, image_usage_flag, width, height);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    //     /* ---------------------------------------------------------*/
  }

  std::unique_ptr<EspDepthBlock> EspDepthBlock::build(EspDepthBlockFormat format,
                                                      EspSampleCountFlag sample_count_flag,
                                                      EspImageUsageFlag image_usage_flag,
                                                      uint32_t width,
                                                      uint32_t height)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    return std::make_unique<VulkanDepthBlock>(format, sample_count_flag, image_usage_flag, width, height);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp
