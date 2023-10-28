#ifndef RENDER_API_ESP_PIPELINE_HH
#define RENDER_API_ESP_PIPELINE_HH

#include "esppch.hh"

// libs
#include "volk.h"

/* ---------------------------------------------------------*/
/* --------- TEMPORARY PIPELINE FOR DEMO -------------------*/
/* ---------------------------------------------------------*/

namespace esp
{
  struct ExampleVertex
  {
    glm::vec2 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
    static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
  };

  struct ExamplePipelineConfigInfo
  {
    ExamplePipelineConfigInfo()                                            = default;
    ExamplePipelineConfigInfo(const ExamplePipelineConfigInfo&)            = delete;
    ExamplePipelineConfigInfo& operator=(const ExamplePipelineConfigInfo&) = delete;

    std::vector<VkVertexInputBindingDescription> m_binding_descriptions{};
    std::vector<VkVertexInputAttributeDescription> m_attribute_descriptions{};

    VkPipelineViewportStateCreateInfo m_viewport_info;
    VkPipelineInputAssemblyStateCreateInfo m_input_assembly_info;
    VkPipelineRasterizationStateCreateInfo m_rasterization_info;
    VkPipelineMultisampleStateCreateInfo m_multisample_info;
    VkPipelineColorBlendAttachmentState m_color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo m_color_blend_info;
    VkPipelineDepthStencilStateCreateInfo m_depth_stencil_info;
    std::vector<VkDynamicState> m_dynamic_states;
    VkPipelineDynamicStateCreateInfo m_dynamic_state_info;
    VkPipelineLayout m_pipeline_layout = nullptr;
    VkRenderPass m_render_pass         = nullptr;
    uint32_t subpass                   = 0;
  };

  class ExamplePipelineLayout
  {
   private:
    VkPipelineLayout m_pipeline_layout;

   public:
    ExamplePipelineLayout(VkPipelineLayout pipeline_layout) : m_pipeline_layout{ pipeline_layout } {}

    ExamplePipelineLayout(const ExamplePipelineLayout&)            = delete;
    ExamplePipelineLayout& operator=(const ExamplePipelineLayout&) = delete;

    ~ExamplePipelineLayout();
  };

  class ExamplePipeline
  {
   public:
    class Builder
    {
     private:
      std::string m_vert_shader_path;
      std::string m_frag_shader_path;

     public:
      Builder() = default;

      Builder& set_vert_shader_path(const std::string& path);
      Builder& set_frag_shader_path(const std::string& path);
      std::unique_ptr<ExamplePipelineLayout> build_pipeline_layout(ExamplePipelineConfigInfo& pipeline_config);
      std::unique_ptr<ExamplePipeline> build_pipeline(ExamplePipelineConfigInfo& pipeline_config);
    };

   private:
    VkPipeline m_graphics_pipeline;
    VkShaderModule m_vert_shader_module;
    VkShaderModule m_frag_shader_module;

   public:
    ExamplePipeline(const std::string& shader_vert_path,
                    const std::string& shader_frag_path,
                    const ExamplePipelineConfigInfo& config_info);
    ~ExamplePipeline();

    ExamplePipeline(const ExamplePipeline&)            = delete;
    ExamplePipeline& operator=(const ExamplePipeline&) = delete;

    void bind();

    static void default_pipeline_config_info(ExamplePipelineConfigInfo& config_info);
    static void enable_alpha_blending(ExamplePipelineConfigInfo& config_info);

   private:
    static std::vector<char> read_file(const std::string& file_path);

    void create_graphics_pipeline(const std::string& shader_vert_path,
                                  const std::string& shader_frag_path,
                                  const ExamplePipelineConfigInfo& config_info);
    void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);
  };
} // namespace esp

#endif // RENDER_API_ESP_PIPELINE_HH
