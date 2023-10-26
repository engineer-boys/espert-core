#include "VulkanSwapChain.hh"
#include "VulkanContext.hh"
#include "VulkanDevice.hh"

// std
#include <array>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

/* --------------------------------------------------------- */
/* ---------------- CLASS IMPLEMENTATION ------------------- */
/* --------------------------------------------------------- */

namespace esp
{
  bool VulkanSwapChain::s_first_initialization = true;

  std::unique_ptr<VulkanSwapChain> VulkanSwapChain::create(VkExtent2D window_extent)
  {
    return std::unique_ptr<VulkanSwapChain>(new VulkanSwapChain(window_extent));
  }

  std::unique_ptr<VulkanSwapChain> VulkanSwapChain::recreate(VkExtent2D window_extent,
                                                             std::shared_ptr<VulkanSwapChain> previous)
  {
    return std::unique_ptr<VulkanSwapChain>(new VulkanSwapChain(window_extent, previous));
  }

  VkResult VulkanSwapChain::acquire_next_image(uint32_t* image_index)
  {
    vkWaitForFences(m_device, 1, &m_in_flight_fences[m_current_frame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(m_device,
                                            m_swap_chain,
                                            std::numeric_limits<uint64_t>::max(),
                                            m_image_available_semaphores[m_current_frame], // must be a not signaled
                                                                                           // semaphore
                                            VK_NULL_HANDLE,
                                            image_index);

    return result;
  }

  VkResult VulkanSwapChain::submit_command_buffers(const VkCommandBuffer* buffers, uint32_t* image_index)
  {
    auto& context_data = VulkanContext::get_context_data();

    if (m_images_in_flight[*image_index] != VK_NULL_HANDLE)
    {
      vkWaitForFences(m_device, 1, &m_images_in_flight[*image_index], VK_TRUE, UINT64_MAX);
    }
    m_images_in_flight[*image_index] = m_in_flight_fences[m_current_frame];

    VkSubmitInfo submit_info = {};
    submit_info.sType        = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[]      = { m_image_available_semaphores[m_current_frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount     = 1;
    submit_info.pWaitSemaphores        = wait_semaphores;
    submit_info.pWaitDstStageMask      = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = buffers;

    VkSemaphore signal_semaphores[]  = { m_render_finished_semaphores[m_current_frame] };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores    = signal_semaphores;

    vkResetFences(m_device, 1, &m_in_flight_fences[m_current_frame]);
    if (vkQueueSubmit(context_data.m_graphics_queue, 1, &submit_info, m_in_flight_fences[m_current_frame]) !=
        VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to submit draw command buffer");
      throw std::runtime_error("Failed to submit draw command buffer");
    }

    VkPresentInfoKHR present_info = {};
    present_info.sType            = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = signal_semaphores;

    VkSwapchainKHR swap_chains[] = { m_swap_chain };
    present_info.swapchainCount  = 1;
    present_info.pSwapchains     = swap_chains;

    present_info.pImageIndices = image_index;

    auto result = vkQueuePresentKHR(context_data.m_present_queue, &present_info);

    m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
  }

  VulkanSwapChain::VulkanSwapChain(VkExtent2D window_extent) : m_window_extent{ window_extent }
  {
    m_device = VulkanDevice::get_logical_device();

    init();
  }

  VulkanSwapChain::VulkanSwapChain(VkExtent2D window_extent, std::shared_ptr<VulkanSwapChain> previous) :
      m_window_extent{ window_extent }, m_old_swap_chain{ previous }
  {
    m_device = VulkanDevice::get_logical_device();

    init();

    m_old_swap_chain = nullptr; // cleanup old swap chain since it's no longer needed
  }

  void VulkanSwapChain::terminate()
  {
    for (auto image_view : m_swap_chain_image_views)
    {
      vkDestroyImageView(m_device, image_view, nullptr);
    }
    m_swap_chain_image_views.clear();

    if (m_swap_chain != nullptr)
    {
      vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr);
      m_swap_chain = nullptr;
    }

    for (int i = 0; i < m_depth_images.size(); i++)
    {
      vkDestroyImageView(m_device, m_depth_image_views[i], nullptr);
      vkDestroyImage(m_device, m_depth_images[i], nullptr);
      vkFreeMemory(m_device, m_depth_image_memories[i], nullptr);
    }

    for (auto framebuffer : m_swap_chain_framebuffers)
    {
      vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }

    vkDestroyRenderPass(m_device, m_render_pass, nullptr);

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      vkDestroySemaphore(m_device, m_render_finished_semaphores[i], nullptr);
      vkDestroySemaphore(m_device, m_image_available_semaphores[i], nullptr);
      vkDestroyFence(m_device, m_in_flight_fences[i], nullptr);
    }
  }

  void VulkanSwapChain::init()
  {
    create_swap_chain();
    create_image_views();
    create_render_pass();
    create_depth_resources();
    create_framebuffers();
    create_sync_objects();
  }

  void VulkanSwapChain::create_swap_chain()
  {
    auto& context_data = VulkanContext::get_context_data();

    SwapChainSupportDetails swap_chain_support = context_data.m_swap_chain_support_details;

    VkSurfaceFormatKHR surface_format = choose_swap_chain_surface_format(swap_chain_support.m_formats);
    VkPresentModeKHR present_mode     = choose_swap_chain_present_mode(swap_chain_support.m_present_modes);
    VkExtent2D extent                 = choose_swap_chain_extent(swap_chain_support.m_capabilities);

    uint32_t image_count = swap_chain_support.m_capabilities.minImageCount + 1;
    if (swap_chain_support.m_capabilities.maxImageCount > 0 &&
        image_count > swap_chain_support.m_capabilities.maxImageCount)
    {
      image_count = swap_chain_support.m_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface                  = context_data.m_surface;

    create_info.minImageCount    = image_count;
    create_info.imageFormat      = surface_format.format;
    create_info.imageColorSpace  = surface_format.colorSpace;
    create_info.imageExtent      = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices      = context_data.m_queue_family_indices;
    uint32_t queue_family_indices[] = { indices.m_graphics_family, indices.m_present_family };

    if (indices.m_graphics_family != indices.m_present_family)
    {
      create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
      create_info.queueFamilyIndexCount = 2;
      create_info.pQueueFamilyIndices   = queue_family_indices;
    }
    else
    {
      create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
      create_info.queueFamilyIndexCount = 0;       // Optional
      create_info.pQueueFamilyIndices   = nullptr; // Optional
    }

    create_info.preTransform   = swap_chain_support.m_capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    create_info.presentMode = present_mode;
    create_info.clipped     = VK_TRUE;

    create_info.oldSwapchain = m_old_swap_chain != nullptr ? m_old_swap_chain->m_swap_chain : VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create swap chain");
      throw std::runtime_error("Failed to create swap chain");
    }

    if (m_old_swap_chain != nullptr) { m_old_swap_chain->terminate(); }

    // we only specified a minimum number of images in the swap chain, so the
    // implementation is allowed to create a swap chain with more. That's why
    // we'll first query the final number of images with
    // vkGetSwapchainImagesKHR, then resize the container and finally call it
    // again to retrieve the handles.
    vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, nullptr);
    m_swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, m_swap_chain_images.data());

    m_swap_chain_image_format = surface_format.format;
    m_swap_chain_extent       = extent;
  }

  void VulkanSwapChain::create_image_views()
  {
    m_swap_chain_image_views.resize(m_swap_chain_images.size());
    for (size_t i = 0; i < m_swap_chain_images.size(); i++)
    {
      VkImageViewCreateInfo view_info{};
      view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      view_info.image                           = m_swap_chain_images[i];
      view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
      view_info.format                          = m_swap_chain_image_format;
      view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
      view_info.subresourceRange.baseMipLevel   = 0;
      view_info.subresourceRange.levelCount     = 1;
      view_info.subresourceRange.baseArrayLayer = 0;
      view_info.subresourceRange.layerCount     = 1;

      if (vkCreateImageView(m_device, &view_info, nullptr, &m_swap_chain_image_views[i]) != VK_SUCCESS)
      {
        ESP_CORE_ERROR("Failed to create texture image view");
        throw std::runtime_error("Failed to create texture image view");
      }
    }
  }

  void VulkanSwapChain::create_render_pass()
  {
    VkAttachmentDescription depth_attachment{};
    depth_attachment.format         = find_depth_format();
    depth_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription color_attachment = {};
    color_attachment.format                  = get_swap_chain_image_format();
    color_attachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout             = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment            = 0;
    color_attachment_ref.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass    = {};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask       = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { color_attachment, depth_attachment };
    VkRenderPassCreateInfo render_pass_info            = {};
    render_pass_info.sType                             = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount                   = static_cast<uint32_t>(attachments.size());
    render_pass_info.pAttachments                      = attachments.data();
    render_pass_info.subpassCount                      = 1;
    render_pass_info.pSubpasses                        = &subpass;
    render_pass_info.dependencyCount                   = 1;
    render_pass_info.pDependencies                     = &dependency;

    if (vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create render pass");
      throw std::runtime_error("Failed to create render pass");
    }
  }

  void VulkanSwapChain::create_framebuffers()
  {
    m_swap_chain_framebuffers.resize(image_count());
    for (size_t i = 0; i < image_count(); i++)
    {
      std::array<VkImageView, 2> attachments = { m_swap_chain_image_views[i], m_depth_image_views[i] };

      VkExtent2D swap_chain_extent             = get_swap_chain_extent();
      VkFramebufferCreateInfo framebuffer_info = {};
      framebuffer_info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebuffer_info.renderPass              = m_render_pass;
      framebuffer_info.attachmentCount         = static_cast<uint32_t>(attachments.size());
      framebuffer_info.pAttachments            = attachments.data();
      framebuffer_info.width                   = swap_chain_extent.width;
      framebuffer_info.height                  = swap_chain_extent.height;
      framebuffer_info.layers                  = 1;

      if (vkCreateFramebuffer(m_device, &framebuffer_info, nullptr, &m_swap_chain_framebuffers[i]) != VK_SUCCESS)
      {
        ESP_CORE_ERROR("Failed to create framebuffer");
        throw std::runtime_error("Failed to create framebuffer");
      }
    }
  }

  void VulkanSwapChain::create_depth_resources()
  {
    VkFormat depth_format        = find_depth_format();
    m_swap_chain_depth_format    = depth_format;
    VkExtent2D swap_chain_extent = get_swap_chain_extent();

    m_depth_images.resize(image_count());
    m_depth_image_memories.resize(image_count());
    m_depth_image_views.resize(image_count());

    // TODO: verify 2 depth images
    auto& context_data = VulkanContext::get_context_data();

    for (int i = 0; i < m_depth_images.size(); i++)
    {
      VkImageCreateInfo image_info{};
      image_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      image_info.imageType     = VK_IMAGE_TYPE_2D;
      image_info.extent.width  = swap_chain_extent.width;
      image_info.extent.height = swap_chain_extent.height;
      image_info.extent.depth  = 1;
      image_info.mipLevels     = 1;
      image_info.arrayLayers   = 1;
      image_info.format        = depth_format;
      image_info.tiling        = VK_IMAGE_TILING_OPTIMAL;
      image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      image_info.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
      image_info.samples       = VK_SAMPLE_COUNT_1_BIT;
      image_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
      image_info.flags         = 0;

      VulkanDevice::get_instance().create_image_with_info(image_info,
                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                          m_depth_images[i],
                                                          m_depth_image_memories[i]);

      VkImageViewCreateInfo view_info{};
      view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      view_info.image                           = m_depth_images[i];
      view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
      view_info.format                          = depth_format;
      view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
      view_info.subresourceRange.baseMipLevel   = 0;
      view_info.subresourceRange.levelCount     = 1;
      view_info.subresourceRange.baseArrayLayer = 0;
      view_info.subresourceRange.layerCount     = 1;

      if (vkCreateImageView(m_device, &view_info, nullptr, &m_depth_image_views[i]) != VK_SUCCESS)
      {
        ESP_CORE_ERROR("Failed to create texture image view");
        throw std::runtime_error("Failed to create texture image view");
      }
    }
  }

  void VulkanSwapChain::create_sync_objects()
  {
    m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
    m_images_in_flight.resize(image_count(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags             = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      if (vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS ||
          vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS ||
          vkCreateFence(m_device, &fence_info, nullptr, &m_in_flight_fences[i]) != VK_SUCCESS)
      {
        ESP_CORE_ERROR("Failed to create synchronization objects for a frame");
        throw std::runtime_error("Failed to create synchronization objects for a frame");
      }
    }
  }

  VkFormat VulkanSwapChain::find_depth_format()
  {
    auto& context_data = esp::VulkanContext::get_context_data();

    return VulkanDevice::get_instance().find_supported_format(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  }

  VkSurfaceFormatKHR
  VulkanSwapChain::choose_swap_chain_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
  {
    for (const auto& available_format : available_formats)
    {
      if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
          available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      {
        return available_format;
      }
    }

    return available_formats[0];
  }

  VkPresentModeKHR
  VulkanSwapChain::choose_swap_chain_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes)
  {
    /*for (const auto& available_present_mode : available_present_modes)
    {
      if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
      {
        log_chosen_swap_chain_present_mode("Mailbox");
        return available_present_mode;
      }
    }*/

    log_chosen_swap_chain_present_mode("V-Sync");
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D VulkanSwapChain::choose_swap_chain_extent(const VkSurfaceCapabilitiesKHR& capabilities)
  {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) { return capabilities.currentExtent; }
    else
    {
      VkExtent2D actual_extent = m_window_extent;
      actual_extent.width =
          std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
      actual_extent.height = std::max(capabilities.minImageExtent.height,
                                      std::min(capabilities.maxImageExtent.height, actual_extent.height));

      return actual_extent;
    }
  }

  void VulkanSwapChain::log_chosen_swap_chain_present_mode(const std::string& mode)
  {
    if (s_first_initialization)
    {
      ESP_CORE_INFO("Present mode: {0}", mode);
      s_first_initialization = false;
    }
  }
} // namespace esp