#include "EspCommandBuffer.hh"

#include "Platform/Vulkan/PipelineOrdering/VulkanCommandBuffer.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  std::unique_ptr<EspCommandBufferId> EspCommandBuffer::begin_only_once()
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
    //     // #if defined(OPENGL_PLATFORM)
    //     // #elif defined(VULKAN_PLATFORM)
    VkCommandBuffer command_buffer = VulkanWorkOrchestrator::begin_single_time_commands();
    return std::make_unique<VulkanCommandBufferId>(command_buffer);
    //     // #else
    //     // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    //     // #endif
    //     /* ---------------------------------------------------------*/
  }

  void EspCommandBuffer::end_only_once(std::unique_ptr<EspCommandBufferId> command_buffer_id)
  {
    //     /* ---------------------------------------------------------*/
    //     /* ------------- PLATFORM DEPENDENT ------------------------*/
    //     /* ---------------------------------------------------------*/
    //     // #if defined(OPENGL_PLATFORM)
    //     // #elif defined(VULKAN_PLATFORM)
    VulkanWorkOrchestrator::end_single_time_commands(
        static_cast<VulkanCommandBufferId*>(command_buffer_id.get())->m_command_buffer);
    //     // #else
    //     // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    //     // #endif
    //     /* ---------------------------------------------------------*/
  }
} // namespace esp
