#include "VulkanCommandHandler.hh"
#include "VulkanContext.hh"
#include "VulkanDevice.hh"
#include "VulkanFrameManager.hh"

namespace esp
{
  VulkanCommandHandler* VulkanCommandHandler::s_instance = nullptr;

  std::unique_ptr<VulkanCommandHandler> VulkanCommandHandler::create()
  {
    return std::unique_ptr<VulkanCommandHandler>(new VulkanCommandHandler());
  }

  VulkanCommandHandler::~VulkanCommandHandler() { s_instance = nullptr; }

  void VulkanCommandHandler::init() { create_command_pool(); }

  void VulkanCommandHandler::terminate()
  {
    vkDestroyCommandPool(VulkanDevice::get_logical_device(), m_command_pool, nullptr);
  }

  void VulkanCommandHandler::draw(uint32_t vertex_count)
  {
    vkCmdDraw(VulkanFrameManager::get_current_command_buffer(), vertex_count, 1, 0, 0);
  }

  void VulkanCommandHandler::draw(uint32_t vertex_count, uint32_t instance_count)
  {
    vkCmdDraw(VulkanFrameManager::get_current_command_buffer(), vertex_count, instance_count, 0, 0);
  }

  void VulkanCommandHandler::draw_indexed(uint32_t index_count)
  {
    vkCmdDrawIndexed(VulkanFrameManager::get_current_command_buffer(), index_count, 1, 0, 0, 0);
  }

  void VulkanCommandHandler::draw_indexed(uint32_t index_count, uint32_t instance_count)
  {
    vkCmdDrawIndexed(VulkanFrameManager::get_current_command_buffer(), index_count, instance_count, 0, 0, 0);
  }

  VkCommandBuffer VulkanCommandHandler::create_command_buffer()
  {
    VkCommandBufferAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // primary - can be submitted to device
                                                           // queue but can't be called by other
                                                           // command buffers
    allocate_info.commandPool        = s_instance->m_command_pool;
    allocate_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer{};
    if (vkAllocateCommandBuffers(VulkanDevice::get_logical_device(), &allocate_info, &command_buffer) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate command buffers");
      throw std::runtime_error("Failed to allocate command buffers");
    }

    return command_buffer;
  }

  void VulkanCommandHandler::free_command_buffer(VkCommandBuffer command_buffer)
  {
    vkFreeCommandBuffers(VulkanDevice::get_logical_device(), s_instance->m_command_pool, 1, &command_buffer);
  }

  VkCommandBuffer VulkanCommandHandler::begin_single_time_commands()
  {
    auto command_buffer = create_command_buffer();

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);
    return command_buffer;
  }

  void VulkanCommandHandler::end_single_time_commands(VkCommandBuffer command_buffer)
  {
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = &command_buffer;

    auto& context_data = VulkanContext::get_context_data();

    vkQueueSubmit(context_data.m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(context_data.m_graphics_queue);

    vkFreeCommandBuffers(VulkanDevice::get_logical_device(), s_instance->m_command_pool, 1, &command_buffer);
  }

  VulkanCommandHandler::VulkanCommandHandler()
  {
    ESP_ASSERT(VulkanCommandHandler::s_instance == nullptr, "Vulkan command handler already exists")
    s_instance = this;
  }

  void VulkanCommandHandler::create_command_pool()
  {
    auto& context_data = VulkanContext::get_context_data();

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex        = context_data.m_queue_family_indices.m_graphics_family;
    pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(VulkanDevice::get_logical_device(), &pool_info, nullptr, &m_command_pool) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create command pool");
      throw std::runtime_error("Failed to create command pool");
    }
  }
} // namespace esp