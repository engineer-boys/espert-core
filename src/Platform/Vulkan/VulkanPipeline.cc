#include "VulkanPipeline.hh"

// libs
#include "volk.h"

// Platform
#include "VulkanDevice.hh"
#include "VulkanFrameManager.hh"

namespace esp
{
  VulkanPipeline::VulkanPipeline(VkPipelineLayout pipeline_layout,
                                 VkPipeline graphics_pipeline,
                                 std::unique_ptr<VulkanUniformManager> uniform_manager) :
      m_pipeline_layout{ pipeline_layout },
      m_graphics_pipeline{ graphics_pipeline }, m_uniform_manager{ std::move(uniform_manager) }
  {
  }

  VulkanPipeline::~VulkanPipeline()
  {
    vkDestroyPipelineLayout(VulkanDevice::get_logical_device(), m_pipeline_layout, nullptr);
    vkDestroyPipeline(VulkanDevice::get_logical_device(), m_graphics_pipeline, nullptr);
  }

  void VulkanPipeline::attach_uniforms() const { m_uniform_manager->attach_descriptor_set_package(m_pipeline_layout); }

  void VulkanPipeline::VulkanPipeline::attach() const
  {
    vkCmdBindPipeline(VulkanFrameManager::get_current_command_buffer(),
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_graphics_pipeline);
  }
  void VulkanPipeline::update_buffer_uniform(uint32_t set, uint32_t binding, uint32_t size, void* data)
  {
    m_uniform_manager->write(set, binding, size, data);
  }

  void VulkanPipeline::update_buffer_uniform(uint32_t set, uint32_t binding, uint32_t elem, uint32_t size, void* data)
  {
    m_uniform_manager->write(set, binding, elem, size, data);
  }

} // namespace esp
