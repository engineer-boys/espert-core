#include "EspGuiRenderPlan.hh"
#include "Platform/Vulkan/RenderPlans/VulkanGuiRenderPlan.hh"

namespace esp
{
  std::unique_ptr<EspGuiRenderPlan> EspGuiRenderPlan::create()
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/

#if ESP_USE_VULKAN
    return std::make_unique<VulkanGuiRenderPlan>();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp