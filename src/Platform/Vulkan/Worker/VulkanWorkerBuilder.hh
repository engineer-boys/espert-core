#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_BUILDER_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_BUILDER_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Resources/EspShader.hh"
#include "Core/RenderAPI/Worker/EspWorkerBuilder.hh"

// Platform
#include "Platform/Vulkan/Uniforms/EspUniformDataStorage.hh"

namespace esp
{
  struct PipelineStageData
  {
    VkShaderModule shader_module                                     = {};
    VkPipelineShaderStageCreateInfo shader_stage_create_info         = {};
    std::vector<VkSpecializationMapEntry> specialization_map_entries = {};
    void* specialization_data                                        = nullptr;
    VkSpecializationInfo specialization_info                         = {};
  };

  class VulkanWorkerBuilder : public EspWorkerBuilder
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    std::unordered_map<EspShaderStage, PipelineStageData> m_pipeline_stage_data_map;

    VkPipelineVertexInputStateCreateInfo m_vertex_input_info;
    /* These must exist until the pipeline is built. */
    std::vector<VkVertexInputBindingDescription> m_binding_descriptions{};
    std::vector<VkVertexInputAttributeDescription> m_attribute_descriptions{};

    bool m_is_pipeline_layout = false;
    VkPipelineLayout m_pipeline_layout; /* it will be moved to the graphic pipieline. */
    std::unique_ptr<EspUniformDataStorage> m_uniform_data_storage;

    std::vector<VkFormat> m_color_attachment_formats;

    struct
    {
      bool m_enable = false;
      EspCompareOp m_compare_op;
      EspDepthBlockFormat m_format;
    } m_depth_test;

    struct
    {
      EspSampleCountFlag m_sample_count_flag = EspSampleCountFlag::ESP_SAMPLE_COUNT_1_BIT;
    } m_multisampling;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanWorkerBuilder();
    ~VulkanWorkerBuilder();

    VulkanWorkerBuilder(const VulkanWorkerBuilder&)            = delete;
    VulkanWorkerBuilder& operator=(const VulkanWorkerBuilder&) = delete;

    virtual void enable_depth_test(EspDepthBlockFormat format, EspCompareOp compare_op) override;
    virtual void enable_multisampling(EspSampleCountFlag sample_count_flag) override;

    virtual void set_attachment_formats(std::vector<EspBlockFormat> formats) override;

    virtual void set_shaders(std::unique_ptr<SpirvResource> spirv_resource) override;

    virtual void set_specialization(const SpecializationConstantMap& spec_const_map) override;

    virtual void set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts) override;
    virtual void set_worker_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data) override;

    virtual std::unique_ptr<EspWorker> build_worker() override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_BUILDER_HH */