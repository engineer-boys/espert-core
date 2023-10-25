#include "VulkanFrameScheduler.hh"
#include "VulkanCommandHandler.hh"
#include "VulkanContext.hh"

// std
#include <array>
#include <stdexcept>

namespace esp
{
  std::unique_ptr<VulkanFrameScheduler> VulkanFrameScheduler::create()
  {
    return std::unique_ptr<VulkanFrameScheduler>(new VulkanFrameScheduler());
  }

  void VulkanFrameScheduler::init(esp::EspWindow& window)
  {
    recreate_swap_chain(window);
    create_command_buffers();
  }

  VulkanFrameScheduler::~VulkanFrameScheduler() { free_command_buffers(); }

  void VulkanFrameScheduler::begin_frame()
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

    VulkanCommandHandler::set_current_command_buffer(command_buffer);
  }

  void VulkanFrameScheduler::end_frame()
  {
    ESP_ASSERT(m_frame_started, "Cannot call end_frame while frame is not in progress")

    auto command_buffer = get_current_command_buffer();
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to record command buffer");
      throw std::runtime_error("Failed to record command buffer");
    }

    auto result = m_swap_chain->submit_command_buffers(&command_buffer, &m_current_image_index);
    // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) { recreate_swap_chain(); } //TODO: create
    // on resize event
    if (result != VK_ERROR_OUT_OF_DATE_KHR && result != VK_SUBOPTIMAL_KHR && result != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to present swap chain image");
      throw std::runtime_error("Failed to present swap chain image");
    }

    m_frame_started       = false;
    m_current_frame_index = (m_current_frame_index + 1) % VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
  }

  void VulkanFrameScheduler::begin_swap_chain_render_pass()
  {
    ESP_ASSERT(m_frame_started,
               "Cannot call begin_swap_chain_render_pass while frame is not in "
               "progress")

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass  = m_swap_chain->get_render_pass();
    render_pass_info.framebuffer = m_swap_chain->get_frame_buffer(m_current_image_index);
    // renderArea - area where shader loads and stores will take place
    render_pass_info.renderArea.offset = { 0, 0 };
    render_pass_info.renderArea.extent = m_swap_chain->get_swap_chain_extent();

    // clear_values - values the frame buffer will use to clear itself
    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color        = { 0.1f, 0.1f, 0.1f, 1.0f }; // color attachment
    clear_values[1].depthStencil = { 1.0f, 0 };                // depth attachment

    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues    = clear_values.data();

    VkCommandBuffer current_command_buffer = get_current_command_buffer();

    vkCmdBeginRenderPass(current_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<float>(m_swap_chain->get_swap_chain_extent().width);
    viewport.height   = static_cast<float>(m_swap_chain->get_swap_chain_extent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ { 0, 0 }, m_swap_chain->get_swap_chain_extent() };
    vkCmdSetViewport(current_command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(current_command_buffer, 0, 1, &scissor);
  }

  void VulkanFrameScheduler::end_swap_chain_render_pass()
  {
    ESP_ASSERT(m_frame_started, "Cannot call end_swap_chain_render_pass while frame is in progress")

    vkCmdEndRenderPass(get_current_command_buffer());
  }

  VulkanFrameScheduler::VulkanFrameScheduler()
  {
    auto& context_data = VulkanContext::get_context_data();
    m_device           = context_data.m_device;
  }

  void VulkanFrameScheduler::create_command_buffers()
  {
    m_command_buffers.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

    for (auto& buffer : m_command_buffers)
    {
      buffer = VulkanCommandHandler::create_command_buffer();
    }
  }

  void VulkanFrameScheduler::free_command_buffers()
  {
    for (auto& buffer : m_command_buffers)
    {
      VulkanCommandHandler::free_command_buffer(buffer);
    }
    m_command_buffers.clear();
  }

  void VulkanFrameScheduler::recreate_swap_chain(EspWindow& window)
  {
    // as long as 1 dimension is 0 (example: minimizing window) we wait
    auto extent = window.get_extent();
    while (extent.width == 0 || extent.height == 0)
    {
      extent = window.get_extent();
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_device); // wait until the current swap
                                // chain is not being used

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
} // namespace esp