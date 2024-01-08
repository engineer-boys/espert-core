#include "VulkanRenderPlan.hh"

#include "Platform/Vulkan/PipelineOrdering/VulkanCommandBuffer.hh"
#include "Platform/Vulkan/Work/VulkanSwapChain.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  VulkanRenderPlan::VulkanRenderPlan()
  {
    m_out_command_buffers.resize(VulkanWorkOrchestrator::get_number_of_command_buffers());
    for (int cb_idx = 0; cb_idx < m_out_command_buffers.size(); cb_idx++)
    {
      m_out_command_buffers[cb_idx] = VulkanWorkOrchestrator::get_command_buffer(cb_idx);
    }

    m_end_src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    m_end_dst_stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  }

  VulkanRenderPlan::~VulkanRenderPlan()
  {
    // m_out_command_buffers shouldn't be free because VulkanWorkOrchestrator is
    // responsible for deleting swapchain commandbuffers.
  }

  void VulkanRenderPlan::add_block(std::shared_ptr<EspBlock> block)
  {
    m_blocks.push_back(std::static_pointer_cast<VulkanBlock>(block));
  }

  void VulkanRenderPlan::add_depth_block(std::shared_ptr<EspDepthBlock> depth_block)
  {
    m_depth_block = std::static_pointer_cast<VulkanDepthBlock>(depth_block);
  }

  void VulkanRenderPlan::build()
  {
    m_width  = m_blocks[0]->get_width();
    m_height = m_blocks[0]->get_height();

    // ................................
    // init berrier's infos for beginning of rendering

    for (int i = 0; i < m_blocks.size(); i++)
    {
      VkImageMemoryBarrier image_memory_barrier = {};
      image_memory_barrier.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      image_memory_barrier.dstAccessMask        = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      image_memory_barrier.oldLayout            = VK_IMAGE_LAYOUT_UNDEFINED;
      image_memory_barrier.newLayout            = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      image_memory_barrier.image                = m_blocks[i]->get_image();
      image_memory_barrier.subresourceRange     = {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1,
      };
      m_begin_barriers_infos.push_back(image_memory_barrier);

      if (m_blocks[i]->is_resolvable())
      {
        image_memory_barrier.image = m_blocks[i]->get_resolve_image();
        m_begin_barriers_infos.push_back(image_memory_barrier);
      }
    }

    if (m_depth_block)
    {
      m_depth_begin_barrier_info                  = {};
      m_depth_begin_barrier_info.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      m_depth_begin_barrier_info.srcAccessMask    = 0;
      m_depth_begin_barrier_info.dstAccessMask    = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      m_depth_begin_barrier_info.oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
      m_depth_begin_barrier_info.newLayout        = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      m_depth_begin_barrier_info.image            = m_depth_block->get_image();
      m_depth_begin_barrier_info.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
    }

    // ................................
    // init berrier's infos for end of rendering
    if (m_new_layout != EspImageLayout::ESP_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
      m_end_src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
      m_end_dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }

    m_end_barriers_infos.resize(m_blocks.size());
    for (int i = 0; i < m_blocks.size(); i++)
    {
      m_end_barriers_infos[i].sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      m_end_barriers_infos[i].srcAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      m_end_barriers_infos[i].oldLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      m_end_barriers_infos[i].newLayout        = static_cast<VkImageLayout>(m_new_layout);
      m_end_barriers_infos[i].subresourceRange = {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1,
      };

      m_end_barriers_infos[i].image =
          m_blocks[i]->is_resolvable() ? m_blocks[i]->get_resolve_image() : m_blocks[i]->get_image();
    }

    // ................................
    // init infor about attachments for rendering
    m_color_attachment_infos.resize(m_blocks.size());
    for (int i = 0; i < m_blocks.size(); i++)
    {
      m_color_attachment_infos[i].sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
      m_color_attachment_infos[i].imageView   = m_blocks[i]->get_image_view();
      m_color_attachment_infos[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      m_color_attachment_infos[i].loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
      m_color_attachment_infos[i].storeOp     = VK_ATTACHMENT_STORE_OP_STORE;

      if (m_blocks[i]->is_resolvable())
      {
        m_color_attachment_infos[i].resolveImageView   = m_blocks[i]->get_resolve_image_view();
        m_color_attachment_infos[i].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        m_color_attachment_infos[i].resolveMode        = VK_RESOLVE_MODE_AVERAGE_BIT;
      }

      auto color                                   = m_blocks[i]->get_clear_color();
      m_color_attachment_infos[i].clearValue.color = { color.x, color.y, color.z, 1.0f };
    }

    m_rendering_info                      = {};
    m_rendering_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    m_rendering_info.renderArea           = { 0, 0, m_width, m_height };
    m_rendering_info.layerCount           = 1;
    m_rendering_info.colorAttachmentCount = m_color_attachment_infos.size();
    m_rendering_info.pColorAttachments    = m_color_attachment_infos.data();

    if (m_depth_block)
    {
      m_depth_stencil_attachment_info                         = {};
      m_depth_stencil_attachment_info.sType                   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
      m_depth_stencil_attachment_info.imageView               = m_depth_block->get_image_view();
      m_depth_stencil_attachment_info.imageLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      m_depth_stencil_attachment_info.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
      m_depth_stencil_attachment_info.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
      m_depth_stencil_attachment_info.clearValue.depthStencil = { 1.0f, 0 };

      m_rendering_info.pDepthAttachment   = &m_depth_stencil_attachment_info;
      m_rendering_info.pStencilAttachment = VK_NULL_HANDLE;
    }
  }

  void VulkanRenderPlan::set_command_buffer(EspCommandBufferId* id)
  {
    for (int cb_idx = 0; cb_idx < m_out_command_buffers.size(); cb_idx++)
    {
      m_out_command_buffers[cb_idx] = static_cast<VulkanCommandBufferId*>(id)->m_command_buffer;
    }
  }

  void VulkanRenderPlan::begin_plan()
  {
    auto frame_idx = VulkanSwapChain::get_current_frame_index();
    // transition color and depth images for drawing
    {
      // color attachement
      vkCmdPipelineBarrier(m_out_command_buffers[frame_idx],
                           VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,             // srcStageMask
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           static_cast<uint32_t>(m_begin_barriers_infos.size()), // imageMemoryBarrierCount
                           m_begin_barriers_infos.data()                         // pImageMemoryBarriers
      );

      // depth attachement
      if (m_depth_block && m_depth_block->need_transition())
      {
        vkCmdPipelineBarrier(m_out_command_buffers[frame_idx],
                             VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                             VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &m_depth_begin_barrier_info);

        m_depth_block->off_transition_need();
      }
    }

    VulkanWorkOrchestrator::begin_rendering(m_out_command_buffers[frame_idx], &m_rendering_info);

    // start recording commands (vkCmd).....
  }

  void VulkanRenderPlan::end_plan()
  {
    // end recording commands (vkCmd).....
    auto frame_idx = VulkanSwapChain::get_current_frame_index();
    VulkanWorkOrchestrator::end_rendering(m_out_command_buffers[frame_idx]);

    // transition color image to presentation

    vkCmdPipelineBarrier(m_out_command_buffers[frame_idx],
                         m_end_src_stage_mask, // srcStageMask
                         m_end_dst_stage_mask, // dstStageMask
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         static_cast<uint32_t>(m_end_barriers_infos.size()), // imageMemoryBarrierCount
                         m_end_barriers_infos.data()                         // pImageMemoryBarriers
    );
  }
} // namespace esp
