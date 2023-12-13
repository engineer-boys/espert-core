#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_BUILDER_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_BUILDER_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"

// Resources
#include "Core/Resources/ShaderProperty.hh"

// Platform
#include "Platform/Vulkan/Uniforms/EspUniformDataStorage.hh"

namespace esp
{
  class VulaknPipelineBuilder : public EspPipelineBuilder
  {
   private:
    std::unordered_map<ShaderStage, VkShaderModule> m_shader_modules;
    std::unordered_map<ShaderStage, VkPipelineShaderStageCreateInfo> m_pipeline_shader_info;

    VkPipelineVertexInputStateCreateInfo m_vertex_input_info;

    std::vector<VkDescriptorSetLayout> m_descriptor_set_layouts;
    std::vector<VkPushConstantRange> m_push_constant_ranges;
    /* These must exist until the pipeline is built. */
    std::vector<VkVertexInputBindingDescription> m_binding_descriptions{};
    std::vector<VkVertexInputAttributeDescription> m_attribute_descriptions{};

    bool m_is_pipeline_layout = false;
    VkPipelineLayout m_pipeline_layout; /* it will be moved to the graphic pipieline. */
    std::unique_ptr<EspUniformDataStorage> m_uniform_data_storage;

   public:
    VulaknPipelineBuilder();
    ~VulaknPipelineBuilder();

    virtual void set_shader(ShaderStage stage, std::vector<uint32_t> shader_source) override;

    virtual void set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts) override;
    virtual void set_pipeline_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data) override;

    virtual std::unique_ptr<EspPipeline> build_pipeline() override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_BUILDER_HH */