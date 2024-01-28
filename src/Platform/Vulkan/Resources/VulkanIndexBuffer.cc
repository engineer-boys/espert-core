#include "VulkanIndexBuffer.hh"
#include "Platform/Vulkan/RenderPlans/VulkanCommandBuffer.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  std::unique_ptr<VulkanIndexBuffer> VulkanIndexBuffer::create(uint32_t* indices, uint32_t index_count)
  {
    std::unique_ptr<VulkanIndexBuffer> index_buffer = std::unique_ptr<VulkanIndexBuffer>(new VulkanIndexBuffer());
    index_buffer->m_index_buffer =
        VulkanBuffer::create_and_fill((void*)indices,
                                      sizeof(indices[0]),
                                      index_count,
                                      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    return index_buffer;
  }

  void VulkanIndexBuffer::attach()
  {
    vkCmdBindIndexBuffer(VulkanWorkOrchestrator::get_current_command_buffer(),
                         m_index_buffer->get_buffer(),
                         0,
                         VK_INDEX_TYPE_UINT32);
  }

  void VulkanIndexBuffer::attach(EspCommandBufferId* id)
  {
    vkCmdBindIndexBuffer(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer,
                         m_index_buffer->get_buffer(),
                         0,
                         VK_INDEX_TYPE_UINT32);
  }
} // namespace esp