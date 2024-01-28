#include "EspCommandBuffer.hh"

#include "Platform/Vulkan/RenderPlans/VulkanCommandBuffer.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  std::unique_ptr<EspCommandBufferId> EspCommandBuffer::begin_only_once()
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VkCommandBuffer command_buffer = VulkanWorkOrchestrator::begin_single_time_commands();
    return std::make_unique<VulkanCommandBufferId>(command_buffer);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    //     /* ---------------------------------------------------------*/
  }

  void EspCommandBuffer::end_only_once(std::unique_ptr<EspCommandBufferId> command_buffer_id)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    VulkanWorkOrchestrator::end_single_time_commands(
        static_cast<VulkanCommandBufferId*>(command_buffer_id.get())->m_command_buffer);
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp
