#ifndef RENDERER_ESP_PIPELINE_H_
#define RENDERER_ESP_PIPELINE_H_

#include "EspDevice.hh"

// std
#include <string>
#include <vector>

namespace esp
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
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
		VkRenderPass m_render_pass = nullptr;
		uint32_t subpass = 0;
	};

	class EspPipeline
	{
	 private:
		EspDevice& m_device;
		VkPipeline m_graphics_pipeline;
		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;

	 public:
		EspPipeline(
			EspDevice& device,
			const std::string& shader_vert_path,
			const std::string& shader_frag_path,
			const PipelineConfigInfo& config_info);
		~EspPipeline();

		EspPipeline(const EspPipeline&) = delete;
		EspPipeline& operator=(const EspPipeline&) = delete;

		void bind(VkCommandBuffer command_buffer);

		static void create_default_pipeline_config_info(PipelineConfigInfo& config_info);
		static void enableAlphaBlending(PipelineConfigInfo& config_info);

	 private:
		static std::vector<char> read_file(const std::string& file_path);

		void create_graphics_pipeline(
			const std::string& shader_vert_path,
			const std::string& shader_frag_path,
			const PipelineConfigInfo& config_info);

		void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);
	};
}

#endif //RENDERER_ESP_PIPELINE_H_
