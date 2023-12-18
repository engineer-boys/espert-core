#include "EspProductPlan.hh"

#include "Platform/Vulkan/PipelineOrdering/VulkanFinalProductPlan.hh"
#include "Platform/Vulkan/PipelineOrdering/VulkanProductPlan.hh"

namespace esp
{

  std::unique_ptr<EspProductPlan> EspProductPlan::build()
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
    //     // #if defined(OPENGL_PLATFORM)
    //     // #elif defined(VULKAN_PLATFORM)
    return std::make_unique<VulkanProductPlan>();
    //     // #else
    //     // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    //     // #endif
    //     /* ---------------------------------------------------------*/
  }

  std::unique_ptr<EspProductPlan> EspProductPlan::build_final(EspSampleCountFlag sample_count_flag)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
    //     // #if defined(OPENGL_PLATFORM)
    //     // #elif defined(VULKAN_PLATFORM)
    auto plan = std::make_unique<VulkanFinalProductPlan>();
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
