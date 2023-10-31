#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_HH

// Render API
#include "Core/RenderAPI/EspPipeline.hh"

// Platform
#include "VulkanUniformManager.hh"
#include "VulkanUniformMetaData.hh"

namespace esp
{
  class VulkanPipeline : public EspPipeline
  {
   private:
    VkPipelineLayout m_pipeline_layout;
    VkPipeline m_graphics_pipeline;
    std::unique_ptr<VulkanUniformManager> m_uniform_manager;

   public:
    VulkanPipeline(VkPipelineLayout pipeline_layout,
                   VkPipeline graphics_pipeline,
                   std::unique_ptr<VulkanUniformManager> uniform_manager);
    ~VulkanPipeline();

    virtual void attach() const override;
    virtual void attach_uniforms() const override;

    virtual void update_buffer_uniform(uint32_t set, uint32_t binding, uint32_t size, void* data) override;
    virtual void
    update_buffer_uniform(uint32_t set, uint32_t binding, uint32_t elem, uint32_t size, void* data) override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_HH */