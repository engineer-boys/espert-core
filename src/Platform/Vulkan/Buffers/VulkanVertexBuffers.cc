#include "VulkanVertexBuffers.hh"
#include "Platform/Vulkan/VulkanFrameManager.hh"

namespace esp
{
  std::unique_ptr<VulkanVertexBuffers> VulkanVertexBuffers::create()
  {
    return std::unique_ptr<VulkanVertexBuffers>(new VulkanVertexBuffers());
  }

  void VulkanVertexBuffers::add(void* data, uint32_t vertex_size, uint32_t vertex_count)
  {
    auto vulkan_buffer =
        VulkanBuffer::create_and_fill(data,
                                      vertex_size,
                                      vertex_count,
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_buffers.push_back(vulkan_buffer->get_buffer());
    m_offsets.push_back(0);
    m_vertex_buffers.push_back(std::move(vulkan_buffer));
  }

  void VulkanVertexBuffers::attach()
  {
    ESP_ASSERT(!m_vertex_buffers.empty(), "Can't bind vertex buffers if there is no buffers to bind")

    vkCmdBindVertexBuffers(VulkanFrameManager::get_current_command_buffer(),
                           0,
                           m_vertex_buffers.size(),
                           m_buffers.data(),
                           m_offsets.data());
  }
} // namespace esp