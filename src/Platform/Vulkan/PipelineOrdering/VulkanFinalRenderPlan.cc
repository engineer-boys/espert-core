#include "VulkanFinalRenderPlan.hh"

#include "Platform/Vulkan/Work/VulkanSwapChain.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  void VulkanFinalRenderPlan::VulkanColorBuffer::terminate()
  {
    vkDestroyImageView(VulkanDevice::get_logical_device(), m_image_view, nullptr);

    vkDestroyImage(VulkanDevice::get_logical_device(), m_image, nullptr);
    vkFreeMemory(VulkanDevice::get_logical_device(), m_image_memory, nullptr);
  }
} // namespace esp

namespace esp
{
  VulkanFinalRenderPlan::VulkanFinalRenderPlan(glm::vec3 clear_color) : m_clear_color{ clear_color } {}

  VulkanFinalRenderPlan::~VulkanFinalRenderPlan()
  {
    if (m_color_buffer.is_enable) { m_color_buffer.terminate(); }
  }

  void VulkanFinalRenderPlan::enable_resolve_block(EspSampleCountFlag sample_count_flag)
  {
    auto [width, height]  = VulkanWorkOrchestrator::get_swap_chain_extent();
    EspBlockFormat format = static_cast<EspBlockFormat>(*(VulkanSwapChain::get_swap_chain_image_format()));

    VulkanResourceManager::create_image(width,
                                        height,
                                        1,
                                        static_cast<VkSampleCountFlagBits>(sample_count_flag),
                                        static_cast<VkFormat>(format),
                                        VK_IMAGE_TILING_OPTIMAL,
                                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                        1,
                                        {},
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        m_color_buffer.m_image,
                                        m_color_buffer.m_image_memory);

    m_color_buffer.m_image_view = VulkanResourceManager::create_image_view(m_color_buffer.m_image,
                                                                           static_cast<VkFormat>(format),
                                                                           VK_IMAGE_ASPECT_COLOR_BIT,
                                                                           1);

    m_color_buffer.is_enable = true;
  }

  void VulkanFinalRenderPlan::add_depth_block(std::shared_ptr<EspDepthBlock> depth_block)
  {
    m_depth_block = std::static_pointer_cast<VulkanDepthBlock>(depth_block);
  }

  void VulkanFinalRenderPlan::build()
  {
    auto [width, height] = VulkanWorkOrchestrator::get_swap_chain_extent();
    m_width              = width;
    m_height             = height;
  }

  void VulkanFinalRenderPlan::begin_plan()
  {
    // transition color and depth images for drawing
    {
      // color attachement
      VulkanWorkOrchestrator::insert_image_memory_barrier_to_current_cmdbuffer(
          VulkanSwapChain::get_current_image(),
          0,
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
          VK_IMAGE_LAYOUT_UNDEFINED,
          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
          VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          {
              .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel   = 0,
              .levelCount     = 1,
              .baseArrayLayer = 0,
              .layerCount     = 1,
          });

      // for multisampling for final product plan
      if (m_color_buffer.is_enable)
      {
        VulkanWorkOrchestrator::insert_image_memory_barrier_to_current_cmdbuffer(
            m_color_buffer.m_image,
            0,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            });
      }

      // depth attachement
      if (m_depth_block && m_depth_block->need_transition())
      {
        VulkanWorkOrchestrator::insert_image_memory_barrier_to_current_cmdbuffer(
            m_depth_block->get_image(),
            0,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 });

        m_depth_block->off_transition_need();
      }
    }

    VkRenderingAttachmentInfoKHR color_attachment_info = {};
    if (m_color_buffer.is_enable)
    {
      color_attachment_info.sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
      color_attachment_info.imageView          = m_color_buffer.m_image_view;
      color_attachment_info.imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      color_attachment_info.loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR;
      color_attachment_info.storeOp            = VK_ATTACHMENT_STORE_OP_STORE;
      color_attachment_info.resolveImageView   = VulkanSwapChain::get_current_image_view();
      color_attachment_info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      color_attachment_info.resolveMode        = VK_RESOLVE_MODE_AVERAGE_BIT;
      color_attachment_info.clearValue.color   = { m_clear_color.x, m_clear_color.y, m_clear_color.z, 0.0f };
    }
    else
    {
      color_attachment_info.sType            = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
      color_attachment_info.imageView        = VulkanSwapChain::get_current_image_view();
      color_attachment_info.imageLayout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      color_attachment_info.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
      color_attachment_info.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
      color_attachment_info.clearValue.color = { m_clear_color.x, m_clear_color.y, m_clear_color.z, 0.0f };
    }

    VkRenderingInfoKHR rendering_info   = {};
    rendering_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.renderArea           = { 0, 0, m_width, m_height };
    rendering_info.layerCount           = 1;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments    = &color_attachment_info;

    if (m_depth_block)
    {
      VkRenderingAttachmentInfoKHR depth_stencil_attachment_info{};
      depth_stencil_attachment_info.sType                   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
      depth_stencil_attachment_info.imageView               = m_depth_block->get_image_view();
      depth_stencil_attachment_info.imageLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      depth_stencil_attachment_info.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
      depth_stencil_attachment_info.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
      depth_stencil_attachment_info.clearValue.depthStencil = { 1.0f, 0 };

      rendering_info.pDepthAttachment   = &depth_stencil_attachment_info;
      rendering_info.pStencilAttachment = VK_NULL_HANDLE;
    }

    VulkanWorkOrchestrator::begin_rendering(&rendering_info);

    // start recording commands (vkCmd).....
  }

  void VulkanFinalRenderPlan::end_plan()
  {
    // end recording commands (vkCmd).....

    VulkanWorkOrchestrator::end_rendering();

    // transition color image to presentation
    VulkanWorkOrchestrator::insert_image_memory_barrier_to_current_cmdbuffer(
        VulkanSwapChain::get_current_image(),
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        0,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        });
  }
} // namespace esp
