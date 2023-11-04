#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_HH

// Render API
#include "Core/RenderAPI/Pipeline/EspPipeline.hh"

// Platform
#include "Platform/Vulkan/Uniforms/EspUniformDataStorage.hh"
#include "Platform/Vulkan/Uniforms/VulkanUniformManager.hh"
#include "Platform/Vulkan/Uniforms/VulkanUniformMetaData.hh"
#include "Platform/Vulkan/VulkanFrameManager.hh"

namespace esp
{
  class VulkanPipeline : public EspPipeline
  {
   private:
    VkPipelineLayout m_pipeline_layout;
    VkPipeline m_graphics_pipeline;
    std::unique_ptr<EspUniformDataStorage> m_uniform_data;

   public:
    VulkanPipeline(VkPipelineLayout pipeline_layout,
                   VkPipeline graphics_pipeline,
                   std::unique_ptr<EspUniformDataStorage> uniform_data);
    ~VulkanPipeline();

    inline virtual void attach() const override
    {
      vkCmdBindPipeline(VulkanFrameManager::get_current_command_buffer(),
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        m_graphics_pipeline);
    }

    virtual std::unique_ptr<EspUniformManager> create_uniform_manager() const override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_HH */