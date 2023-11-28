#include "VulkanVertexBuffers.hh"
#include "Platform/Vulkan/VulkanFrameManager.hh"

namespace esp
{
  std::unique_ptr<VulkanVertexBuffers> VulkanVertexBuffers::create()
  {
    return std::unique_ptr<VulkanVertexBuffers>(new VulkanVertexBuffers());
  }

  void VulkanVertexBuffers::add(void* data, uint32_t vertex_size, uint32_t vertex_count, BufferType type)
  {
    VkMemoryPropertyFlags memory_property_flags;
    switch (type)
    {
    case LOCAL:
      memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      break;
    case VISIBLE:
      memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      break;
    }

    auto vulkan_buffer =
        VulkanBuffer::create_and_fill(data,
                                      vertex_size,
                                      vertex_count,
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      memory_property_flags);

    m_buffers.push_back(vulkan_buffer->get_buffer());
    m_offsets.push_back(0);
    m_vertex_buffers.push_back(std::move(vulkan_buffer));
  }

  void VulkanVertexBuffers::update(uint32_t index,
                                   void* data,
                                   uint32_t instance_size,
                                   uint32_t instance_count,
                                   uint64_t offset)
  {
    auto size = instance_size * instance_count;
    ESP_ASSERT(index < m_vertex_buffers.size(), "Index was out of range")
    ESP_ASSERT(m_vertex_buffers[index]->get_buffer_size() >= size + offset,
               "Data size + offset can't be larger than size of updated buffer")

    m_vertex_buffers[index]->map(size, offset); // TODO: it might be better to map this buffer permanently in 'add'
    m_vertex_buffers[index]->write_to_buffer(data, size, offset);
    m_vertex_buffers[index]->unmap();
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