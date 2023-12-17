#include "VulkanProductPlan.hh"

#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  VulkanProductPlan::VulkanProductPlan() {}

  VulkanProductPlan::~VulkanProductPlan() {}

  void VulkanProductPlan::add_building_block(std::shared_ptr<EspBlock> block)
  {
    m_blocks.push_back(std::static_pointer_cast<VulkanBlock>(block));
  }

  void VulkanProductPlan::begin_plan()
  {
    // TODO: these structures shouldn't be here. There should be created and init in constructor.
    // TODO: add recreate_because_resize_windows() fuction to recreate these structures (this should be done after
    // the 1. TODO).

    // transition color and depth images for drawing
    {
      // color attachement
      std::vector<VkImageMemoryBarrier> image_memory_barriers = {};
      image_memory_barriers.resize(m_blocks.size());

      for (int i = 0; i < m_blocks.size(); i++)
      {
        image_memory_barriers[i].sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barriers[i].dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        image_memory_barriers[i].oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
        image_memory_barriers[i].newLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        image_memory_barriers[i].image            = m_blocks[i]->get_image();
        image_memory_barriers[i].subresourceRange = {
          .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
          .baseMipLevel   = 0,
          .levelCount     = 1,
          .baseArrayLayer = 0,
          .layerCount     = 1,
        };
      }

      vkCmdPipelineBarrier(VulkanWorkOrchestrator::get_current_command_buffer(),
                           VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,             // srcStageMask
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           static_cast<uint32_t>(image_memory_barriers.size()), // imageMemoryBarrierCount
                           image_memory_barriers.data()                         // pImageMemoryBarriers
      );

      // depth attachement
      // ...
    }

    std::vector<VkRenderingAttachmentInfoKHR> color_attachment_infos = {};
    color_attachment_infos.resize(m_blocks.size());
    for (int i = 0; i < m_blocks.size(); i++)
    {
      color_attachment_infos[i].sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
      color_attachment_infos[i].imageView   = m_blocks[i]->get_image_view();
      color_attachment_infos[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      color_attachment_infos[i].loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
      color_attachment_infos[i].storeOp     = VK_ATTACHMENT_STORE_OP_STORE;

      auto color                                 = m_blocks[i]->get_clear_color();
      color_attachment_infos[i].clearValue.color = { color.x, color.y, color.z, 1.0f };
    }

    auto [width, height]                = VulkanWorkOrchestrator::get_swap_chain_extent();
    VkRenderingInfoKHR rendering_info   = {};
    rendering_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.renderArea           = { 0, 0, width, height };
    rendering_info.layerCount           = 1;
    rendering_info.colorAttachmentCount = color_attachment_infos.size();
    rendering_info.pColorAttachments    = color_attachment_infos.data();
    // rendering_info.pDepthAttachment     = &depthStencilAttachment;
    // rendering_info.pStencilAttachment   = &depthStencilAttachment;

    // this->vkCmdbeginRenderingKHR(m_command_buffers[current_frame], &rendering_info);
    VulkanWorkOrchestrator::begin_rendering(&rendering_info);

    // start recording commands (vkCmd).....
  }

  void VulkanProductPlan::end_plan()
  {
    // end recording commands (vkCmd).....

    VulkanWorkOrchestrator::end_rendering();

    // transition color image to presentation

    std::vector<VkImageMemoryBarrier> image_memory_barriers = {};
    image_memory_barriers.resize(m_blocks.size());

    for (int i = 0; i < m_blocks.size(); i++)
    {
      image_memory_barriers[i].sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      image_memory_barriers[i].srcAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      image_memory_barriers[i].oldLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      image_memory_barriers[i].newLayout        = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // this may be wrong
      image_memory_barriers[i].image            = m_blocks[i]->get_image();
      image_memory_barriers[i].subresourceRange = {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1,
      };
    }

    vkCmdPipelineBarrier(VulkanWorkOrchestrator::get_current_command_buffer(),
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStageMask
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,          // dstStageMask
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         static_cast<uint32_t>(image_memory_barriers.size()), // imageMemoryBarrierCount
                         image_memory_barriers.data()                         // pImageMemoryBarriers
    );
  }
} // namespace esp
