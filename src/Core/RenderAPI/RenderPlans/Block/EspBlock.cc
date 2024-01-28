#include "EspBlock.hh"

#include "Core/RenderAPI/Work/EspWorkOrchestrator.hh"
#include "Platform/Vulkan/RenderPlans/Block/VulkanBlock.hh"

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
#if ESP_USE_VULKAN
    auto [width, height] = EspWorkOrchestrator::get_swap_chain_extent();
    return std::make_shared<VulkanBlock>(format, sample_count_flag, width, height, clear_color);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    //     /* ---------------------------------------------------------*/
  }

  std::shared_ptr<EspBlock> EspBlock::build(EspBlockFormat format,
                                            EspSampleCountFlag sample_count_flag,
                                            uint32_t width,
                                            uint32_t height,
                                            glm::vec3 clear_color)
  {
//     /* ---------------------------------------------------------*/
//     /* ------------- PLATFORM DEPENDENT ------------------------*/
//     /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    return std::make_shared<VulkanBlock>(format, sample_count_flag, width, height, clear_color);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    //     /* ---------------------------------------------------------*/
  }

  std::shared_ptr<EspTexture> EspBlock::extract_texture(std::shared_ptr<EspBlock> block)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    auto vulkan_block = std::static_pointer_cast<VulkanBlock>(block);
    return vulkan_block->extract_texture();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp
