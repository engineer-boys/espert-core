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

  std::unique_ptr<EspProductPlan> EspProductPlan::build_final()
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
    //     // #if defined(OPENGL_PLATFORM)
    //     // #elif defined(VULKAN_PLATFORM)
    return std::make_unique<VulkanFinalProductPlan>();
    //     // #else
    //     // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    //     // #endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp
