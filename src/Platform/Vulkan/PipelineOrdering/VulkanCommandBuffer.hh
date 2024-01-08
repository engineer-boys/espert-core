#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_COMMAND_BUFFER_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_COMMAND_BUFFER_HH

#include "esppch.hh"

#include "Core/RenderAPI/PipelineOrdering/EspCommandBuffer.hh"

namespace esp
{
  class VulkanCommandBufferId : public EspCommandBufferId
  {
   public:
    VkCommandBuffer m_command_buffer;

    VulkanCommandBufferId(VkCommandBuffer command_buffer) : m_command_buffer{ command_buffer } {}
  };
} // namespace esp

#endif // PLATFORM_VULKAN_RENDER_API_VULKAN_COMMAND_BUFFER_HH