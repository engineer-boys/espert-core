#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_HH

// Render API
#include "Core/RenderAPI/Worker/EspWorker.hh"

// Platform
#include "Platform/Vulkan/RenderPlans/VulkanCommandBuffer.hh"
#include "Platform/Vulkan/Uniforms/EspUniformDataStorage.hh"
#include "Platform/Vulkan/Uniforms/VulkanUniformManager.hh"
#include "Platform/Vulkan/Uniforms/VulkanUniformMetaData.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  class VulkanWorker : public EspWorker
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    VkPipelineLayout m_pipeline_layout;
    VkPipeline m_graphics_pipeline;
    std::unique_ptr<EspUniformDataStorage> m_uniform_data;

    /* -------------------------- METHODS ---------------------------------- */
   private:
    void set_viewport() const;
    void set_scissors() const;

   public:
    VulkanWorker(VkPipelineLayout pipeline_layout,
                 VkPipeline graphics_pipeline,
                 std::unique_ptr<EspUniformDataStorage> uniform_data);
    ~VulkanWorker();

    VulkanWorker(const VulkanWorker&)            = delete;
    VulkanWorker& operator=(const VulkanWorker&) = delete;

    inline virtual void attach() const override
    {
      vkCmdBindPipeline(VulkanWorkOrchestrator::get_current_command_buffer(),
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        m_graphics_pipeline);
      set_viewport();
      set_scissors();
    }

    inline virtual void only_attach() const override
    {
      vkCmdBindPipeline(VulkanWorkOrchestrator::get_current_command_buffer(),
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        m_graphics_pipeline);
    }

    virtual void set_viewport(EspViewport viewport) override;
    virtual void set_scissors(EspScissorRect scissor_rect) override;

    inline virtual void only_attach(EspCommandBufferId* id) const override
    {
      vkCmdBindPipeline(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        m_graphics_pipeline);
    }

    virtual void set_viewport(EspCommandBufferId* id, EspViewport viewport) override;
    virtual void set_scissors(EspCommandBufferId* id, EspScissorRect scissor_rect) override;

    virtual std::unique_ptr<EspUniformManager> create_uniform_manager(int start_managed_ds = -1,
                                                                      int end_managed_ds   = -1) const override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_HH */