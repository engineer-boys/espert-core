#include "EspRenderPlan.hh"

#include "Platform/Vulkan/PipelineOrdering/VulkanFinalRenderPlan.hh"
#include "Platform/Vulkan/PipelineOrdering/VulkanRenderPlan.hh"

namespace esp
{

  std::unique_ptr<EspRenderPlan> EspRenderPlan::create()
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
    //     // #if defined(OPENGL_PLATFORM)
    //     // #elif defined(VULKAN_PLATFORM)
    return std::make_unique<VulkanRenderPlan>();
    //     // #else
    //     // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    //     // #endif
    //     /* ---------------------------------------------------------*/
  }

  std::unique_ptr<EspRenderPlan> EspRenderPlan::create_final(glm::vec3 clear_color,
                                                             EspSampleCountFlag sample_count_flag)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
    //     // #if defined(OPENGL_PLATFORM)
    //     // #elif defined(VULKAN_PLATFORM)
    auto plan = std::make_unique<VulkanFinalRenderPlan>(clear_color);
    if (sample_count_flag != EspSampleCountFlag::ESP_SAMPLE_COUNT_1_BIT)
    {
      plan->enable_resolve_block(sample_count_flag);
    }
    return plan;
    //     // #else
    //     // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    //     // #endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp
