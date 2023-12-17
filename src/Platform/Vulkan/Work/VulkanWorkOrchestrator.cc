#include "VulkanWorkOrchestrator.hh"
#include "Platform/Vulkan/VulkanContext.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"
#include "VulkanSwapChain.hh"

namespace esp
{
  VulkanWorkOrchestrator* VulkanWorkOrchestrator::s_instance = nullptr;

  std::unique_ptr<VulkanWorkOrchestrator> VulkanWorkOrchestrator::create()
  {
    ESP_ASSERT(VulkanWorkOrchestrator::s_instance == nullptr, "The vulkan work orchestrator already exists!");
    VulkanWorkOrchestrator::s_instance = new VulkanWorkOrchestrator();
    VulkanWorkOrchestrator::s_instance->init();

    return std::unique_ptr<VulkanWorkOrchestrator>{ VulkanWorkOrchestrator::s_instance };
  }

  VulkanWorkOrchestrator::VulkanWorkOrchestrator() {}

  VulkanWorkOrchestrator::~VulkanWorkOrchestrator() {}

  void VulkanWorkOrchestrator::init()
  {
    m_swap_chain = VulkanSwapChain::create();

    create_command_pool();
    create_command_buffers();
    create_sync_objects();
    load_extension_functions();
  }

  void VulkanWorkOrchestrator::terminate()
  {
    ESP_ASSERT(VulkanWorkOrchestrator::s_instance != nullptr, "The vulkan work orchestrator is deleted twice!");

    // Idle until all queues have finish their work

    for (size_t i = 0; i < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
    {
      vkDestroySemaphore(VulkanDevice::get_logical_device(), m_render_finished_semaphores[i], nullptr);
      vkDestroySemaphore(VulkanDevice::get_logical_device(), m_image_available_semaphores[i], nullptr);
      vkDestroyFence(VulkanDevice::get_logical_device(), m_in_flight_fences[i], nullptr);
    }

    vkDestroyCommandPool(VulkanDevice::get_logical_device(), m_command_pool, nullptr);
    m_swap_chain->terminate();

    VulkanWorkOrchestrator::s_instance = nullptr;
  }

  void VulkanWorkOrchestrator::begin_frame()
  {
    auto current_frame = m_swap_chain->m_current_frame;
    vkWaitForFences(VulkanDevice::get_logical_device(),
                    1,
                    &m_in_flight_fences[current_frame],
                    VK_TRUE,
                    std::numeric_limits<uint64_t>::max());
    vkResetFences(VulkanDevice::get_logical_device(), 1, &m_in_flight_fences[current_frame]);

    auto result = m_swap_chain->acquire_next_image(m_image_available_semaphores);
    {
      if (result == VK_ERROR_OUT_OF_DATE_KHR)
      {
        m_swap_chain->resize();
        return;
      }
      else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
      {
        throw std::runtime_error("Failed to acquire swap chain image!");
      }
    }

    vkResetCommandBuffer(m_command_buffers[current_frame], /*VkCommandBufferResetFlagBits*/ 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    ESP_ASSERT(vkBeginCommandBuffer(m_command_buffers[current_frame], &beginInfo) == VK_SUCCESS,
               "Failed to begin recording command buffer!");

    // // transition color and depth images for drawing
    // {
    //   // color attachement
    //   const VkImageMemoryBarrier image_memory_barrier{ .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    //                                                    .dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    //                                                    .oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED,
    //                                                    .newLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    //                                                    .image            = m_swap_chain->get_current_image(),
    //                                                    .subresourceRange = {
    //                                                        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
    //                                                        .baseMipLevel   = 0,
    //                                                        .levelCount     = 1,
    //                                                        .baseArrayLayer = 0,
    //                                                        .layerCount     = 1,
    //                                                    } };

    //   vkCmdPipelineBarrier(m_command_buffers[current_frame],
    //                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,             // srcStageMask
    //                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
    //                        0,
    //                        0,
    //                        nullptr,
    //                        0,
    //                        nullptr,
    //                        1,                    // imageMemoryBarrierCount
    //                        &image_memory_barrier // pImageMemoryBarriers
    //   );

    //   // depth attachement
    //   // ...
    // }

    // VkRenderingAttachmentInfoKHR color_attachment_info = {};
    // color_attachment_info.sType                        = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    // color_attachment_info.imageView                    = m_swap_chain->get_current_image_view();
    // color_attachment_info.imageLayout                  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // color_attachment_info.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // color_attachment_info.storeOp                      = VK_ATTACHMENT_STORE_OP_STORE;
    // color_attachment_info.clearValue.color             = { 0.0f, 5.0f, 5.0f, 0.0f };

    // VkRenderingInfoKHR rendering_info   = {};
    // rendering_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    // rendering_info.renderArea           = { 0,
    //                                         0,
    //                                         m_swap_chain->m_swap_chain_extent.width,
    //                                         m_swap_chain->m_swap_chain_extent.height };
    // rendering_info.layerCount           = 1;
    // rendering_info.colorAttachmentCount = 1;
    // rendering_info.pColorAttachments    = &color_attachment_info;
    // // rendering_info.pDepthAttachment     = &depthStencilAttachment;
    // // rendering_info.pStencilAttachment   = &depthStencilAttachment;

    // this->vkCmdbeginRenderingKHR(m_command_buffers[current_frame], &rendering_info);

    // // start recording commands (vkCmd).....
  }

  void VulkanWorkOrchestrator::end_frame()
  {
    // // end recording commands (vkCmd).....

    auto current_frame = m_swap_chain->m_current_frame;
    // this->vkCmdEndRenderingKHR(m_command_buffers[current_frame]);

    // // transition color image to presentation

    // const VkImageMemoryBarrier image_memory_barrier{ .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    //                                                  .srcAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    //                                                  .oldLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    //                                                  .newLayout        = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    //                                                  .image            = m_swap_chain->get_current_image(),
    //                                                  .subresourceRange = {
    //                                                      .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
    //                                                      .baseMipLevel   = 0,
    //                                                      .levelCount     = 1,
    //                                                      .baseArrayLayer = 0,
    //                                                      .layerCount     = 1,
    //                                                  } };

    // vkCmdPipelineBarrier(m_command_buffers[current_frame],
    //                      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStageMask
    //                      VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,          // dstStageMask
    //                      0,
    //                      0,
    //                      nullptr,
    //                      0,
    //                      nullptr,
    //                      1,                    // imageMemoryBarrierCount
    //                      &image_memory_barrier // pImageMemoryBarriers
    // );

    ESP_ASSERT(vkEndCommandBuffer(m_command_buffers[current_frame]) == VK_SUCCESS, "Failed to record command buffer!");

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[]      = { m_image_available_semaphores[current_frame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &m_command_buffers[current_frame];

    VkSemaphore signalSemaphores[]  = { m_render_finished_semaphores[current_frame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    auto data_context = VulkanContext::get_context_data();
    ESP_ASSERT(vkQueueSubmit(data_context.m_graphics_queue, 1, &submitInfo, m_in_flight_fences[current_frame]) ==
                   VK_SUCCESS,
               "Failed to submit draw command buffer!")

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_swap_chain->m_swap_chain };
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;

    presentInfo.pImageIndices = &m_swap_chain->m_image_index;

    auto result = vkQueuePresentKHR(data_context.m_present_queue, &presentInfo);
    {
      if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_swap_chain_resize)
      {
        m_swap_chain_resize = false;
        m_swap_chain->resize();
      }
      else if (result != VK_SUCCESS) { throw std::runtime_error("Failed to present swap chain image!"); }
    }

    m_swap_chain->go_to_next_frame();
  }

  void VulkanWorkOrchestrator::on_window_resized(WindowResizedEvent& e) { m_swap_chain_resize = true; }

  void VulkanWorkOrchestrator::create_command_pool()
  {
    auto& context_data = VulkanContext::get_context_data();

    VkCommandPoolCreateInfo pool_info = {};

    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT why this?
    pool_info.queueFamilyIndex = context_data.m_queue_family_indices.m_graphics_family.value();

    ESP_ASSERT(vkCreateCommandPool(VulkanDevice::get_logical_device(), &pool_info, nullptr, &m_command_pool) ==
                   VK_SUCCESS,
               "Failed to create command pool");
  }

  void VulkanWorkOrchestrator::create_command_buffers()
  {
    m_command_buffers.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = m_command_pool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_command_buffers.size();

    ESP_ASSERT(vkAllocateCommandBuffers(VulkanDevice::get_logical_device(), &allocInfo, m_command_buffers.data()) ==
                   VK_SUCCESS,
               "Failed to allocate command buffers!");
  }

  void VulkanWorkOrchestrator::create_sync_objects()
  {
    m_image_available_semaphores.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
    m_render_finished_semaphores.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
    m_in_flight_fences.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags             = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
    {
      ESP_ASSERT(vkCreateSemaphore(VulkanDevice::get_logical_device(),
                                   &semaphore_info,
                                   nullptr,
                                   &m_image_available_semaphores[i]) == VK_SUCCESS,
                 "Failed to create semaphores (part 1) for Work Orchestrator");

      ESP_ASSERT(vkCreateSemaphore(VulkanDevice::get_logical_device(),
                                   &semaphore_info,
                                   nullptr,
                                   &m_render_finished_semaphores[i]) == VK_SUCCESS,
                 "Failed to create semaphores (part 2) for Work Orchestrator");

      ESP_ASSERT(vkCreateFence(VulkanDevice::get_logical_device(), &fence_info, nullptr, &m_in_flight_fences[i]) ==
                     VK_SUCCESS,
                 "Failed to create fences for Work Orchestrator")
    }
  }

  void VulkanWorkOrchestrator::load_extension_functions()
  {
    // I used "this" syntax in below fields to emphasise that function pointers are loaded here.
    this->vkCmdbeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(
        vkGetDeviceProcAddr(VulkanDevice::get_logical_device(), "vkCmdBeginRenderingKHR"));

    this->vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(
        vkGetDeviceProcAddr(VulkanDevice::get_logical_device(), "vkCmdEndRenderingKHR"));
  }

  /* ------------- CHANGE THEM !!!!!!!!!!!! -------------- */
  VkCommandBuffer VulkanWorkOrchestrator::begin_single_time_commands()
  {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool        = s_instance->m_command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(VulkanDevice::get_logical_device(), &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);
    return command_buffer;
  }

  void VulkanWorkOrchestrator::end_single_time_commands(VkCommandBuffer command_buffer)
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
} // namespace esp
