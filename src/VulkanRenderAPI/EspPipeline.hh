#ifndef RENDERER_ESP_PIPELINE_H_
#define RENDERER_ESP_PIPELINE_H_

#include "EspDevice.hh"
#include "EspPushConstantData.hh"

// std
#include <string>
#include <vector>

namespace esp
{
  struct PipelineConfigInfo
  {
    PipelineConfigInfo()                                     = default;
    PipelineConfigInfo(const PipelineConfigInfo&)            = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

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

  class EspPipelineLayout
  {
   private:
    VkPipelineLayout m_pipeline_layout;

   public:
    EspPipelineLayout(VkPipelineLayout pipeline_layout) :
        m_pipeline_layout{ pipeline_layout }
    {
    }

    EspPipelineLayout(const EspPipelineLayout&)            = delete;
    EspPipelineLayout& operator=(const EspPipelineLayout&) = delete;

    inline VkPipelineLayout& get_layout() { return m_pipeline_layout; }

    void destroy(EspDevice& device);
  };

  class EspPipeline
  {
   public:
    class Builder
    {
     private:
      EspDevice& m_device;

      std::string m_vert_shader_path;
      std::string m_frag_shader_path;

     public:
      Builder(EspDevice& device) : m_device{ device } {}

      Builder& set_vert_shader_path(const std::string& path);
      Builder& set_frag_shader_path(const std::string& path);
      std::unique_ptr<EspPipelineLayout> build_pipeline_layout(
          PipelineConfigInfo&
              pipeline_config); // TODO: add descriptor set layout
      std::unique_ptr<EspPipelineLayout>
      build_pipeline_layout(PipelineConfigInfo& pipeline_config,
                            VkPushConstantRange push_constant_range);
      std::unique_ptr<EspPipeline>
      build_pipeline(PipelineConfigInfo& pipeline_config,
                     VkRenderPass render_pass);

     private:
      std::unique_ptr<EspPipelineLayout>
      create_pipeline_layout(PipelineConfigInfo& pipeline_config,
                             VkPipelineLayoutCreateInfo create_info);
    };

   private:
    EspDevice& m_device;
    VkPipeline m_graphics_pipeline;
    VkShaderModule m_vert_shader_module;
    VkShaderModule m_frag_shader_module;

   public:
    EspPipeline(EspDevice& device, const std::string& shader_vert_path,
                const std::string& shader_frag_path,
                const PipelineConfigInfo& config_info);
    ~EspPipeline();

    EspPipeline(const EspPipeline&)            = delete;
    EspPipeline& operator=(const EspPipeline&) = delete;

    void bind(VkCommandBuffer command_buffer);

    static void default_pipeline_config_info(PipelineConfigInfo& config_info);
    static void enable_alpha_blending(PipelineConfigInfo& config_info);

   private:
    static std::vector<char> read_file(const std::string& file_path);

    void create_graphics_pipeline(const std::string& shader_vert_path,
                                  const std::string& shader_frag_path,
                                  const PipelineConfigInfo& config_info);
    void create_shader_module(const std::vector<char>& code,
                              VkShaderModule* shader_module);
  };
} // namespace esp

#endif // RENDERER_ESP_PIPELINE_H_
