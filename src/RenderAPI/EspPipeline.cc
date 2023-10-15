#include "EspPipeline.hh"
#include "Scene/Components/Components.hh"

// std
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace esp
{
	EspPipeline::Builder& EspPipeline::Builder::set_vert_shader_path(const std::string& path)
	{
		m_vert_shader_path = path;
		return *this;
	}

	EspPipeline::Builder& EspPipeline::Builder::set_frag_shader_path(const std::string& path)
	{
		m_frag_shader_path = path;
		return *this;
	}

	std::unique_ptr<EspPipelineLayout> EspPipeline::Builder::build_pipeline_layout(
		PipelineConfigInfo& pipeline_config)
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		VkPipelineLayout pipeline_layout{};
		if (vkCreatePipelineLayout(m_device.get_device(), &pipelineLayoutCreateInfo, nullptr, &pipeline_layout) != VK_SUCCESS)
		{
			ESP_CORE_ERROR("Failed to create pipeline layout");
			throw std::runtime_error("Failed to create pipeline layout");
		}

		pipeline_config.m_pipeline_layout = pipeline_layout;

		return std::make_unique<EspPipelineLayout>(pipeline_layout);
	}

	std::unique_ptr<EspPipeline> EspPipeline::Builder::build_pipeline(
		esp::PipelineConfigInfo& pipeline_config,
		VkRenderPass render_pass)
	{
		assert(pipeline_config.m_pipeline_layout != nullptr
			&& "Can't create pipeline before pipeline layout");

		pipeline_config.m_render_pass = render_pass;

		return std::make_unique<EspPipeline>(
			m_device,
			m_vert_shader_path,
			m_frag_shader_path,
			pipeline_config);
	}

	EspPipeline::EspPipeline(
		EspDevice& device,
		const std::string& shader_vert_path,
		const std::string& shader_frag_path,
		const PipelineConfigInfo& config_info
	) : m_device{ device }
	{
		create_graphics_pipeline(shader_vert_path, shader_frag_path, config_info);
	}

	EspPipeline::~EspPipeline()
	{
		vkDestroyShaderModule(m_device.get_device(), m_vert_shader_module, nullptr);
		vkDestroyShaderModule(m_device.get_device(), m_frag_shader_module, nullptr);
		vkDestroyPipeline(m_device.get_device(), m_graphics_pipeline, nullptr);
	}

	std::vector<char> EspPipeline::read_file(const std::string& file_path)
	{
		std::string engine_path = ENGINE_DIR + file_path;
		std::ifstream file(engine_path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			ESP_CORE_ERROR("Failed to open file: {0}", engine_path);
			throw std::runtime_error("Failed to open file: " + engine_path);
		}

		size_t file_size = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(file_size);

		file.seekg(0);
		file.read(buffer.data(), file_size);
		file.close();

		return buffer;
	}

	void EspPipeline::create_graphics_pipeline(
		const std::string& shader_vert_path,
		const std::string& shader_frag_path,
		const PipelineConfigInfo& config_info)
	{
		assert(
			config_info.m_pipeline_layout != VK_NULL_HANDLE
				&& "Cannot create graphics pipeline: no m_pipeline_layout provided on config_info");
		assert(
			config_info.m_render_pass != VK_NULL_HANDLE
				&& "Cannot create graphics pipeline: no m_render_pass  provided on config_info");

		auto shader_vert = read_file(shader_vert_path);
		auto shader_frag = read_file(shader_frag_path);

		create_shader_module(shader_vert, &m_vert_shader_module);
		create_shader_module(shader_frag, &m_frag_shader_module);

		//customize shader functionality
		VkPipelineShaderStageCreateInfo shader_stages[2];
		shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shader_stages[0].module = m_vert_shader_module;
		shader_stages[0].pName = "main";
		shader_stages[0].flags = 0;
		shader_stages[0].pNext = nullptr;
		shader_stages[0].pSpecializationInfo = nullptr;
		shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shader_stages[1].module = m_frag_shader_module;
		shader_stages[1].pName = "main";
		shader_stages[1].flags = 0;
		shader_stages[1].pNext = nullptr;
		shader_stages[1].pSpecializationInfo = nullptr;

		auto& binding_descriptions = config_info.m_binding_descriptions;
		auto& attribute_descriptions = config_info.m_attribute_descriptions;
		//describes how we interprete our vertex buffer data into our graphics pipeline
		VkPipelineVertexInputStateCreateInfo vertex_input_info{};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
		vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_descriptions.size());
		vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();
		vertex_input_info.pVertexBindingDescriptions = binding_descriptions.data();

		VkGraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &config_info.m_input_assembly_info;
		pipeline_info.pViewportState = &config_info.m_viewport_info;
		pipeline_info.pRasterizationState = &config_info.m_rasterization_info;
		pipeline_info.pMultisampleState = &config_info.m_multisample_info;
		pipeline_info.pColorBlendState = &config_info.m_color_blend_info;
		pipeline_info.pDepthStencilState = &config_info.m_depth_stencil_info;
		pipeline_info.pDynamicState = &config_info.m_dynamic_state_info;

		pipeline_info.layout = config_info.m_pipeline_layout;
		pipeline_info.renderPass = config_info.m_render_pass;
		pipeline_info.subpass = config_info.subpass;

		//enables GPU to create new pipeline by deriving from existing one (for performance)
		pipeline_info.basePipelineIndex = -1;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_device.get_device(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline)
			!= VK_SUCCESS)
		{
			ESP_CORE_ERROR("Failed to create graphics pipeline");
			throw std::runtime_error("Failed to create graphics pipeline");
		}
	}

	void EspPipeline::create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module)
	{
		VkShaderModuleCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_device.get_device(), &create_info, nullptr, shader_module) != VK_SUCCESS)
		{
			ESP_CORE_ERROR("Failed to create shader module");
			throw std::runtime_error("Failed to create shader module");
		}
	}

	void EspPipeline::bind(VkCommandBuffer command_buffer)
	{
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);
	}

	void EspPipeline::default_pipeline_config_info(PipelineConfigInfo& config_info)
	{
		//inputAssemblyInfo - defines how the assembler is going to proceed vertices
		config_info.m_input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		config_info.m_input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		config_info.m_input_assembly_info.primitiveRestartEnable = VK_FALSE;

		config_info.m_viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		config_info.m_viewport_info.viewportCount = 1;
		config_info.m_viewport_info.pViewports = nullptr;
		config_info.m_viewport_info.scissorCount = 1;
		config_info.m_viewport_info.pScissors = nullptr;

		//rasterization - breaks up geometry into fragments for each pixel our triangle overlaps
		config_info.m_rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		config_info.m_rasterization_info.depthClampEnable = VK_FALSE;      // 0 <= z <= 1
		config_info.m_rasterization_info.rasterizerDiscardEnable =
			VK_FALSE;    // true - if we want to use only first few stages of the graphics pipeline
		config_info.m_rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;    // describes how we want to draw our triangles
		config_info.m_rasterization_info.lineWidth = 1.0f;
		config_info.m_rasterization_info.cullMode =
			VK_CULL_MODE_NONE;      // discard triangles based on their parent facing (for performance)
		config_info.m_rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		config_info.m_rasterization_info.depthBiasEnable = VK_FALSE;      // for changing depth values
		config_info.m_rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
		config_info.m_rasterization_info.depthBiasClamp = 0.0f;           // Optional
		config_info.m_rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

		//multisampling - relates to how the rasterizer handles the edges of geometry (anti-aliasing)
		config_info.m_multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		config_info.m_multisample_info.sampleShadingEnable = VK_FALSE;
		config_info.m_multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		config_info.m_multisample_info.minSampleShading = 1.0f;           // Optional
		config_info.m_multisample_info.pSampleMask = nullptr;             // Optional
		config_info.m_multisample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
		config_info.m_multisample_info.alphaToOneEnable = VK_FALSE;       // Optional

		//colorBlend - controls how we combine colors in our framebuffer
		config_info.m_color_blend_attachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT;
		config_info.m_color_blend_attachment.blendEnable = VK_FALSE;
		config_info.m_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		config_info.m_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		config_info.m_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		config_info.m_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		config_info.m_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		config_info.m_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		config_info.m_color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		config_info.m_color_blend_info.logicOpEnable = VK_FALSE;
		config_info.m_color_blend_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
		config_info.m_color_blend_info.attachmentCount = 1;
		config_info.m_color_blend_info.pAttachments = &config_info.m_color_blend_attachment;
		config_info.m_color_blend_info.blendConstants[0] = 0.0f;  // Optional
		config_info.m_color_blend_info.blendConstants[1] = 0.0f;  // Optional
		config_info.m_color_blend_info.blendConstants[2] = 0.0f;  // Optional
		config_info.m_color_blend_info.blendConstants[3] = 0.0f;  // Optional

		//depthStencil - configures depth buffer
		config_info.m_depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		config_info.m_depth_stencil_info.depthTestEnable = VK_TRUE;
		config_info.m_depth_stencil_info.depthWriteEnable = VK_TRUE;
		config_info.m_depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
		config_info.m_depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
		config_info.m_depth_stencil_info.minDepthBounds = 0.0f;  // Optional
		config_info.m_depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
		config_info.m_depth_stencil_info.stencilTestEnable = VK_FALSE;
		config_info.m_depth_stencil_info.front = {};  // Optional
		config_info.m_depth_stencil_info.back = {};   // Optional

		config_info.m_dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		config_info.m_dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		config_info.m_dynamic_state_info.pDynamicStates = config_info.m_dynamic_states.data();
		config_info.m_dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(config_info.m_dynamic_states.size());
		config_info.m_dynamic_state_info.flags = 0;

		config_info.m_binding_descriptions = ModelComponent::Vertex::get_binding_descriptions();
		config_info.m_attribute_descriptions = ModelComponent::Vertex::get_attribute_descriptions();
	}

	// this method requires rendering solid objects first and then semi-transparent objects from furthest to closest
	void EspPipeline::enable_alpha_blending(PipelineConfigInfo& config_info)
	{
		//colorBlend - controls how we combine colors in our framebuffer
		config_info.m_color_blend_attachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT;
		config_info.m_color_blend_attachment.blendEnable = VK_TRUE;
		config_info.m_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		config_info.m_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		config_info.m_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		config_info.m_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		config_info.m_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		config_info.m_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	}
}