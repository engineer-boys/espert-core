#include "VulkanVertexBuffer.hh"
#include "Platform/Vulkan/PipelineOrdering/VulkanCommandBuffer.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  std::unique_ptr<VulkanVertexBuffer> VulkanVertexBuffer::create(void* data,
                                                                 uint32_t vertex_size,
                                                                 uint32_t vertex_count,
                                                                 BufferType type)
  {
    std::unique_ptr<VulkanVertexBuffer> vertex_buffer = std::unique_ptr<VulkanVertexBuffer>(new VulkanVertexBuffer());

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

    vertex_buffer->m_vertex_buffer =
        VulkanBuffer::create_and_fill(data,
                                      vertex_size,
                                      vertex_count,
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      memory_property_flags);

    return vertex_buffer;
  }

  void VulkanVertexBuffer::update(void* data, uint32_t instance_size, uint32_t instance_count, uint64_t offset)
  {
    auto size = instance_size * instance_count;
    ESP_ASSERT(m_vertex_buffer->get_buffer_size() >= size + offset,
               "Data size + offset can't be larger than size of updated buffer")

    m_vertex_buffer->map(size, offset); // TODO: it might be better to map this buffer permanently in 'create'
    m_vertex_buffer->write_to_buffer(data, size, offset);
    m_vertex_buffer->unmap();
  }

  void VulkanVertexBuffer::attach()
  {
    VkBuffer buffers[]     = { m_vertex_buffer->get_buffer() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(VulkanWorkOrchestrator::get_current_command_buffer(), 0, 1, buffers, offsets);
  }

  void VulkanVertexBuffer::attach_instanced(esp::EspVertexBuffer& instance_buffer)
  {
    VkBuffer buffers[]     = { m_vertex_buffer->get_buffer(),
                               dynamic_cast<VulkanVertexBuffer&>(instance_buffer).m_vertex_buffer->get_buffer() };
    VkDeviceSize offsets[] = { 0, 0 };

    vkCmdBindVertexBuffers(VulkanWorkOrchestrator::get_current_command_buffer(), 0, 2, buffers, offsets);
  }

  void VulkanVertexBuffer::attach(EspCommandBufferId* id)
  {
    VkBuffer buffers[]     = { m_vertex_buffer->get_buffer() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer, 0, 1, buffers, offsets);
  }

  void VulkanVertexBuffer::attach_instanced(EspCommandBufferId* id, EspVertexBuffer& instance_buffer)
  {
    VkBuffer buffers[]     = { m_vertex_buffer->get_buffer(),
                               dynamic_cast<VulkanVertexBuffer&>(instance_buffer).m_vertex_buffer->get_buffer() };
    VkDeviceSize offsets[] = { 0, 0 };

    vkCmdBindVertexBuffers(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer, 0, 2, buffers, offsets);
  }
} // namespace esp