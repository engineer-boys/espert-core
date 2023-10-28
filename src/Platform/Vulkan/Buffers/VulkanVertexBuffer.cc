#include "VulkanVertexBuffer.hh"
#include "Platform/Vulkan/VulkanFrameManager.hh"

namespace esp
{
  std::unique_ptr<VulkanVertexBuffer>
  VulkanVertexBuffer::create(void* data, uint32_t vertex_size, uint32_t vertex_count)
  {
    std::unique_ptr<VulkanVertexBuffer> vertex_buffer = std::unique_ptr<VulkanVertexBuffer>(new VulkanVertexBuffer());
    vertex_buffer->m_vertex_buffer =
        VulkanBuffer::create_and_fill(data,
                                      vertex_size,
                                      vertex_count,
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    return vertex_buffer;
  }

  void VulkanVertexBuffer::attach()
  {
    VkBuffer buffers[]     = { m_vertex_buffer->get_buffer() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(VulkanFrameManager::get_current_command_buffer(), 0, 1, buffers, offsets);
  }
} // namespace esp