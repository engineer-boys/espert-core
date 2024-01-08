#include "VulkanPipeline.hh"

// libs
#include "volk.h"

// Platform
#include "Platform/Vulkan/Uniforms/VulkanUniformManager.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  VulkanWorker::VulkanWorker(VkPipelineLayout pipeline_layout,
                             VkPipeline graphics_pipeline,
                             std::unique_ptr<EspUniformDataStorage> uniform_data) :
      m_pipeline_layout{ pipeline_layout },
      m_graphics_pipeline{ graphics_pipeline }, m_uniform_data{ std::move(uniform_data) }
  {
  }

  VulkanWorker::~VulkanWorker()
  {
    vkDestroyPipelineLayout(VulkanDevice::get_logical_device(), m_pipeline_layout, nullptr);
    vkDestroyPipeline(VulkanDevice::get_logical_device(), m_graphics_pipeline, nullptr);
  }

  std::unique_ptr<EspUniformManager> VulkanWorker::create_uniform_manager(int start_managed_ds,
                                                                          int end_managed_ds) const
  {
    ESP_ASSERT(m_uniform_data, "You cannot create EspUniformManager if you didn't define any Descriptor Set!");
    return std::unique_ptr<EspUniformManager>{
      new VulkanUniformManager(*m_uniform_data, m_pipeline_layout, start_managed_ds, end_managed_ds)
    };
  }

  void VulkanWorker::set_viewport() const
  {
    auto [widht, height] = VulkanWorkOrchestrator::get_swap_chain_extent();

    VkViewport viewport{};
    viewport.width    = static_cast<float>(widht);
    viewport.height   = -static_cast<float>(height);
    viewport.x        = 0.0f;
    viewport.y        = static_cast<float>(height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(VulkanWorkOrchestrator::get_current_command_buffer(), 0, 1, &viewport);
  }

  void VulkanWorker::set_viewport(EspCommandBufferId* id, EspViewport viewport)
  {
    VkViewport vk_viewport{};
    vk_viewport.width    = static_cast<float>(viewport.m_width);
    vk_viewport.height   = -static_cast<float>(viewport.m_height);
    vk_viewport.x        = 0.0f;
    vk_viewport.y        = static_cast<float>(viewport.m_height);
    vk_viewport.minDepth = viewport.m_min_depth;
    vk_viewport.maxDepth = viewport.m_max_depth;

    vkCmdSetViewport(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer, 0, 1, &vk_viewport);
  }

  void VulkanWorker::set_scissors() const
  {
    auto [widht, height] = VulkanWorkOrchestrator::get_swap_chain_extent();

    VkRect2D scissor{ { 0, 0 }, { widht, height } };
    vkCmdSetScissor(VulkanWorkOrchestrator::get_current_command_buffer(), 0, 1, &scissor);
  }

  void VulkanWorker::set_scissors(EspCommandBufferId* id, EspScissorRect scissor_rect)
  {
    VkRect2D scissor{ { scissor_rect.m_offset_x, scissor_rect.m_offset_y },
                      { scissor_rect.m_width, scissor_rect.m_height } };

    vkCmdSetScissor(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer, 0, 1, &scissor);
  }
} // namespace esp
