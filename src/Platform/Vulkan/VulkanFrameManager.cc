#include "VulkanFrameManager.hh"
#include "VulkanCommandHandler.hh"
#include "VulkanContext.hh"

// std
#include <array>
#include <stdexcept>

namespace esp
{
  VulkanFrameManager* VulkanFrameManager::s_instance = nullptr;

  std::unique_ptr<VulkanFrameManager> VulkanFrameManager::create()
  {
    return std::unique_ptr<VulkanFrameManager>(new VulkanFrameManager());
  }

  VulkanFrameManager::~VulkanFrameManager() { s_instance = nullptr; }

  void VulkanFrameManager::init()
  {
    recreate_swap_chain();
    create_command_buffers();
  }

  void VulkanFrameManager::begin_frame()
  {
    ESP_ASSERT(!m_frame_started, "Cannot call begin_frame while frame already in progress")

    auto result = m_swap_chain->acquire_next_image(&m_current_image_index);

    // window has changed in such a way that is no longer compatible with the
    // swap chain (example: resizing)
    if (result == VK_ERROR_OUT_OF_DATE_KHR) { return; }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
      ESP_CORE_ERROR("Failed to acquire swap chain image");
      throw std::runtime_error("Failed to acquire swap chain image");
    }

    m_frame_started = true;

    auto command_buffer = get_current_command_buffer();
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to reset command buffer");
      throw std::runtime_error("Failed to reset command buffer");
    }

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to begin recording command buffer");
      throw std::runtime_error("Failed to begin recording command buffer");
    }

    begin_render_pass();
  }

  void VulkanFrameManager::end_frame()
  {
    ESP_ASSERT(m_frame_started, "Cannot call end_frame while frame is not in progress")

    end_render_pass();

    auto command_buffer = get_current_command_buffer();
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to record command buffer");
      throw std::runtime_error("Failed to record command buffer");
    }

    auto result = m_swap_chain->submit_command_buffers(&command_buffer, &m_current_image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) { recreate_swap_chain(); }
    else if (result != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to present swap chain image");
      throw std::runtime_error("Failed to present swap chain image");
    }

    m_frame_started       = false;
    m_current_frame_index = (m_current_frame_index + 1) % VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
  }

  VulkanFrameManager::VulkanFrameManager()
  {
    ESP_ASSERT(VulkanFrameManager::s_instance == nullptr, "Vulkan frame manager already exists")
    s_instance = this;
  }

  void VulkanFrameManager::begin_render_pass()
  {
    ESP_ASSERT(m_frame_started,
               "Cannot call begin_render_pass while frame is not in "
               "progress")

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass  = m_swap_chain->get_render_pass();
    render_pass_info.framebuffer = m_swap_chain->get_frame_buffer(m_current_image_index);
    // renderArea - area where shader loads and stores will take place
    render_pass_info.renderArea.offset = { 0, 0 };
    render_pass_info.renderArea.extent = m_swap_chain->get_extent();

    // clear_values - values the frame buffer will use to clear itself
    std::array<VkClearValue, 2> clear_values = get_clear_values();

    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues    = clear_values.data();

    VkCommandBuffer current_command_buffer = get_current_command_buffer();

    vkCmdBeginRenderPass(current_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    set_viewport();
    set_scissors();
  }

  void VulkanFrameManager::end_render_pass()
  {
    ESP_ASSERT(m_frame_started, "Cannot call end_render_pass while frame is in progress")

    vkCmdEndRenderPass(get_current_command_buffer());
  }

  void VulkanFrameManager::terminate()
  {
    VulkanDevice::complete_queues();
    free_command_buffers();
    m_swap_chain->terminate();
  }

  void VulkanFrameManager::on_window_resized(WindowResizedEvent& e)
  {
    EspFrameManager::on_window_resized(e);
    recreate_swap_chain();
  }

  bool VulkanFrameManager::is_frame_in_progress() { return s_instance->m_frame_started; };

  VkCommandBuffer VulkanFrameManager::get_current_command_buffer()
  {
    ESP_ASSERT(s_instance->m_frame_started, "Cannot get command buffer if frame hasn't started")
    return s_instance->m_command_buffers[s_instance->m_current_frame_index];
  }

  int VulkanFrameManager::get_current_frame_index()
  {
    ESP_ASSERT(s_instance->m_frame_started, "Cannot get frame index if frame hasn't started")
    return s_instance->m_current_frame_index;
  }

  VkRenderPass VulkanFrameManager::get_swap_chain_render_pass() { return s_instance->m_swap_chain->get_render_pass(); }

  std::pair<uint32_t, uint32_t> VulkanFrameManager::get_swap_chain_extent()
  {
    return { s_instance->m_swap_chain->get_extent_width(), s_instance->m_swap_chain->get_extent_height() };
  }

  float VulkanFrameManager::get_swap_chain_extent_aspect_ratio()
  {
    return s_instance->m_swap_chain->get_extent_aspect_ratio();
  }

  void VulkanFrameManager::create_command_buffers()
  {
    m_command_buffers.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

    for (auto& buffer : m_command_buffers)
    {
      buffer = VulkanCommandHandler::create_command_buffer();
    }
  }

  void VulkanFrameManager::free_command_buffers()
  {
    for (auto& buffer : m_command_buffers)
    {
      VulkanCommandHandler::free_command_buffer(buffer);
    }
    m_command_buffers.clear();
  }

  void VulkanFrameManager::recreate_swap_chain()
  {
    // as long as 1 dimension is 0 (example: minimizing window) we wait
    auto extent = get_window_extent();
    while (extent.width == 0 || extent.height == 0)
    {
      extent = get_window_extent();
      glfwWaitEvents();
    }

    VulkanDevice::complete_queues();

    if (m_swap_chain == nullptr) { m_swap_chain = VulkanSwapChain::create(extent); }
    else
    {
      std::shared_ptr<VulkanSwapChain> old_swap_chain = std::move(m_swap_chain);
      m_swap_chain                                    = VulkanSwapChain::recreate(extent, old_swap_chain);

      if (!old_swap_chain->compare_swap_chain_formats(*m_swap_chain))
      {
        ESP_CORE_ERROR("Swap chain image/depth format has changed");
        throw std::runtime_error("Swap chain image/depth format has changed");
      }
    }
  }

  std::array<VkClearValue, 2> VulkanFrameManager::get_clear_values()
  {
    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = { m_clear_color.x, m_clear_color.y, m_clear_color.z, m_clear_color.w }; // color attachment
    clear_values[1].depthStencil = { m_depth_stencil.m_depth, m_depth_stencil.m_stencil };          // depth attachment

    return clear_values;
  }

  void VulkanFrameManager::set_viewport()
  {
    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = static_cast<float>(m_swap_chain->get_extent().height);
    viewport.width    = static_cast<float>(m_swap_chain->get_extent().width);
    viewport.height   = -static_cast<float>(m_swap_chain->get_extent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(get_current_command_buffer(), 0, 1, &viewport);
  }

  void VulkanFrameManager::set_scissors()
  {
    VkRect2D scissor{ { 0, 0 }, m_swap_chain->get_extent() };
    vkCmdSetScissor(get_current_command_buffer(), 0, 1, &scissor);
  }

  VkExtent2D VulkanFrameManager::get_window_extent()
  {
    return { static_cast<uint32_t>(m_window_width), static_cast<uint32_t>(m_window_height) };
  }
} // namespace esp
