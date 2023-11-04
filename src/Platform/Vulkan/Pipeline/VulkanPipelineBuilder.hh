#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_BUILDER_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_BUILDER_HH

// libs
#include "volk.h"

// Render API
#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"

// Platform
#include "Platform/Vulkan/Uniforms/EspUniformDataStorage.hh"

namespace esp
{
  class VulaknPipelineBuilder : public EspPipelineBuilder
  {
   private:
    bool m_is_vertex_shader_module = false;
    VkShaderModule m_vertex_shader_module;
    VkPipelineShaderStageCreateInfo m_vertex_shader_info;

    bool m_is_fragment_shader_module = false;
    VkShaderModule m_fragment_shader_module;
    VkPipelineShaderStageCreateInfo m_fragment_shader_info;

    VkPipelineVertexInputStateCreateInfo m_vertex_input_info;
    /* These must exist until the pipeline is built. */
    std::vector<VkVertexInputBindingDescription> m_binding_descriptions{};
    std::vector<VkVertexInputAttributeDescription> m_attribute_descriptions{};

    bool m_is_pipeline_layout = false;
    VkPipelineLayout m_pipeline_layout; /* it will be moved to the graphic pipieline. */
    std::unique_ptr<EspUniformDataStorage> m_uniform_data_storage;

   public:
    VulaknPipelineBuilder();
    ~VulaknPipelineBuilder();

    virtual void set_shaders(std::string path_vertex_shr, std::string path_fragment_shr) override;
    virtual void set_vertex_shader(std::string path_vertex_shr) override;
    virtual void set_fragment_shader(std::string path_fragment_shr) override;

    virtual void set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts) override;
    virtual void set_pipeline_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data) override;

    virtual std::unique_ptr<EspPipeline> build_pipeline() override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_PIPELINE_BUILDER_HH */