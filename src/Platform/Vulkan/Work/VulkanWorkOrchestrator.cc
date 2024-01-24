#include "VulkanWorkOrchestrator.hh"
#include "Platform/Vulkan/VulkanContext.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"
#include "VulkanSwapChain.hh"

namespace esp
{
  VulkanWorkOrchestrator* VulkanWorkOrchestrator::s_instance = nullptr;

  std::unique_ptr<VulkanWorkOrchestrator> VulkanWorkOrchestrator::create(EspPresentationMode presentation_mode)
  {
    ESP_ASSERT(VulkanWorkOrchestrator::s_instance == nullptr, "The vulkan work orchestrator already exists!");
    VulkanWorkOrchestrator::s_instance = new VulkanWorkOrchestrator();
    VulkanWorkOrchestrator::s_instance->init(presentation_mode);

    return std::unique_ptr<VulkanWorkOrchestrator>{ VulkanWorkOrchestrator::s_instance };
  }

  VulkanWorkOrchestrator::VulkanWorkOrchestrator() {}

  VulkanWorkOrchestrator::~VulkanWorkOrchestrator() {}

  void VulkanWorkOrchestrator::init(EspPresentationMode presentation_mode)
  {
    m_swap_chain = VulkanSwapChain::create(presentation_mode);

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

    ESP_ASSERT(m_swap_chain->acquire_next_image(m_image_available_semaphores) == VK_SUCCESS,
               "Failed to acquire swap chain image!");
    vkResetCommandBuffer(m_command_buffers[current_frame], /*VkCommandBufferResetFlagBits*/ 0);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    ESP_ASSERT(vkBeginCommandBuffer(m_command_buffers[current_frame], &begin_info) == VK_SUCCESS,
               "Failed to begin recording command buffer!");
  }

  void VulkanWorkOrchestrator::end_frame()
  {
    auto current_frame = m_swap_chain->m_current_frame;
    ESP_ASSERT(vkEndCommandBuffer(m_command_buffers[current_frame]) == VK_SUCCESS, "Failed to record command buffer!");

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[]      = { m_image_available_semaphores[current_frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount     = 1;
    submit_info.pWaitSemaphores        = wait_semaphores;
    submit_info.pWaitDstStageMask      = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = &m_command_buffers[current_frame];

    VkSemaphore signal_semaphores[]  = { m_render_finished_semaphores[current_frame] };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores    = signal_semaphores;

    auto data_context = VulkanContext::get_context_data();
    ESP_ASSERT(vkQueueSubmit(data_context.m_graphics_queue, 1, &submit_info, m_in_flight_fences[current_frame]) ==
                   VK_SUCCESS,
               "Failed to submit draw command buffer!")

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = signal_semaphores;

    VkSwapchainKHR swap_chains[] = { m_swap_chain->m_swap_chain };
    present_info.swapchainCount  = 1;
    present_info.pSwapchains     = swap_chains;

    present_info.pImageIndices = &m_swap_chain->m_image_index;

    ESP_ASSERT(vkQueuePresentKHR(data_context.m_present_queue, &present_info) == VK_SUCCESS,
               "Failed to present swap chain image!");
    m_swap_chain->go_to_next_frame();
  }

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

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool        = m_command_pool;
    alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = (uint32_t)m_command_buffers.size();

    ESP_ASSERT(vkAllocateCommandBuffers(VulkanDevice::get_logical_device(), &alloc_info, m_command_buffers.data()) ==
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
    this->m_vkCmdbeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(
        vkGetDeviceProcAddr(VulkanDevice::get_logical_device(), "vkCmdBeginRenderingKHR"));

    this->m_vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(
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
