#include "EspRenderPlan.hh"

#include "Platform/Vulkan/RenderPlans/VulkanFinalRenderPlan.hh"
#include "Platform/Vulkan/RenderPlans/VulkanRenderPlan.hh"

namespace esp
{

  std::unique_ptr<EspRenderPlan> EspRenderPlan::create()
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/

#if ESP_USE_VULKAN
    return std::make_unique<VulkanRenderPlan>();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    //     /* ---------------------------------------------------------*/
  }

  std::unique_ptr<EspRenderPlan> EspRenderPlan::create_final(glm::vec3 clear_color,
                                                             EspSampleCountFlag sample_count_flag)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    auto plan = std::make_unique<VulkanFinalRenderPlan>(clear_color);
    if (sample_count_flag != EspSampleCountFlag::ESP_SAMPLE_COUNT_1_BIT)
    {
      plan->enable_resolve_block(sample_count_flag);
    }
    return plan;
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp
