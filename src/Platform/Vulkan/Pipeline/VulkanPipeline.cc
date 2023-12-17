#include "VulkanPipeline.hh"

// libs
#include "volk.h"

// Platform
#include "Platform/Vulkan/Uniforms/VulkanUniformManager.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanFrameManager.hh"

namespace esp
{
  VulkanPipeline::VulkanPipeline(VkPipelineLayout pipeline_layout,
                                 VkPipeline graphics_pipeline,
                                 std::unique_ptr<EspUniformDataStorage> uniform_data) :
      m_pipeline_layout{ pipeline_layout },
      m_graphics_pipeline{ graphics_pipeline }, m_uniform_data{ std::move(uniform_data) }
  {
  }

  VulkanPipeline::~VulkanPipeline()
  {
    vkDestroyPipelineLayout(VulkanDevice::get_logical_device(), m_pipeline_layout, nullptr);
    vkDestroyPipeline(VulkanDevice::get_logical_device(), m_graphics_pipeline, nullptr);
  }

  std::unique_ptr<EspUniformManager> VulkanPipeline::create_uniform_manager(int start_managed_ds,
                                                                            int end_managed_ds) const
  {
    return std::unique_ptr<EspUniformManager>{
      new VulkanUniformManager(*m_uniform_data, m_pipeline_layout, start_managed_ds, end_managed_ds)
    };
  }
} // namespace esp
