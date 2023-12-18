#include "VulkanPipelineBuilder.hh"

// std
#include <fstream>

// Platform
#include "Platform/Vulkan/Uniforms/VulkanUniformMetaData.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/Work/VulkanSwapChain.hh"
#include "VulkanPipeline.hh"

// signatures
static std::vector<char> read_file(const std::string& filename);
static VkShaderModule create_shader_module(const std::vector<char>& code, VkDevice device);

/* --------------------------------------------------------- */
/* ---------------- CLASS IMPLEMENTATION ------------------- */
/* --------------------------------------------------------- */

namespace esp
{
  VulkanWorkerBuilder::VulkanWorkerBuilder() : m_vertex_shader_info{}, m_fragment_shader_info{}, m_vertex_input_info{}
  {
    m_color_attachment_formats.push_back(*(VulkanSwapChain::get_swap_chain_image_format()));
  }

  VulkanWorkerBuilder::~VulkanWorkerBuilder()
  {
    if (m_is_fragment_shader_module)
    {
      vkDestroyShaderModule(VulkanDevice::get_logical_device(), m_fragment_shader_module, nullptr);
    }

    if (m_is_vertex_shader_module)
    {
      vkDestroyShaderModule(VulkanDevice::get_logical_device(), m_vertex_shader_module, nullptr);
    }

    if (m_is_pipeline_layout)
    {
      vkDestroyPipelineLayout(VulkanDevice::get_logical_device(), m_pipeline_layout, nullptr);
    }
  }

  void VulkanWorkerBuilder::enable_depth_test(EspDepthBlockFormat format, EspCompareOp compare_op)
  {
    m_depth_test.m_enable     = true;
    m_depth_test.m_format     = format;
    m_depth_test.m_compare_op = compare_op;
  }

  void VulkanWorkerBuilder::enable_multisampling(EspSampleCountFlag sample_count_flag)
  {
    m_multisampling.m_sample_count_flag = sample_count_flag;
  }

  void VulkanWorkerBuilder::set_attachment_formats(std::vector<EspBlockFormat> formats)
  {
    m_color_attachment_formats.clear();
    for (auto format : formats)
    {
      m_color_attachment_formats.push_back(static_cast<VkFormat>(format));
    }
  }

  void VulkanWorkerBuilder::set_shaders(std::string path_vertex_shr, std::string path_fragment_shr)
  {
    set_vertex_shader(path_vertex_shr);
    set_fragment_shader(path_fragment_shr);
  }

  void VulkanWorkerBuilder::set_vertex_shader(std::string path_vertex_shr)
  {
    auto vertex_shader_code   = read_file(path_vertex_shr);
    m_vertex_shader_module    = create_shader_module(vertex_shader_code, VulkanDevice::get_logical_device());
    m_is_vertex_shader_module = true;

    m_vertex_shader_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_vertex_shader_info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    m_vertex_shader_info.module = m_vertex_shader_module;
    m_vertex_shader_info.pName  = "main";
  }

  void VulkanWorkerBuilder::set_fragment_shader(std::string path_fragment_shr)
  {
    auto fragment_shader_code   = read_file(path_fragment_shr);
    m_fragment_shader_module    = create_shader_module(fragment_shader_code, VulkanDevice::get_logical_device());
    m_is_fragment_shader_module = true;

    m_fragment_shader_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_fragment_shader_info.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    m_fragment_shader_info.module = m_fragment_shader_module;
    m_fragment_shader_info.pName  = "main";
  }

  void VulkanWorkerBuilder::set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts)
  {
    for (auto& vtx_layout : vertex_layouts)
    {
      VkVertexInputBindingDescription binding_description{};

      binding_description.binding   = vtx_layout.m_binding;
      binding_description.stride    = vtx_layout.m_size;
      binding_description.inputRate = static_cast<VkVertexInputRate>(vtx_layout.m_input_rate);
      m_binding_descriptions.push_back(binding_description);

      for (auto& attr : vtx_layout.m_attrs)
      {
        VkVertexInputAttributeDescription attribute_description{};

        attribute_description.binding  = vtx_layout.m_binding;
        attribute_description.location = attr.m_location;
        attribute_description.format   = static_cast<VkFormat>(attr.m_format);
        attribute_description.offset   = attr.m_offset;
        m_attribute_descriptions.push_back(attribute_description);
      }
    }

    m_vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertex_input_info.vertexBindingDescriptionCount   = static_cast<uint32_t>(m_binding_descriptions.size());
    m_vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attribute_descriptions.size());
    m_vertex_input_info.pVertexBindingDescriptions      = m_binding_descriptions.data();
    m_vertex_input_info.pVertexAttributeDescriptions    = m_attribute_descriptions.data();
  }

  void VulkanWorkerBuilder::set_pipeline_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data)
  {
    std::unique_ptr<VulkanUniformMetaData> meta_data(static_cast<VulkanUniformMetaData*>(uniforms_meta_data.release()));

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if (*meta_data)
    {
      m_uniform_data_storage = std::make_unique<EspUniformDataStorage>(std::move(meta_data));
      auto set_layout_count  = m_uniform_data_storage->get_layouts_count();
      auto pushes_count      = m_uniform_data_storage->get_pushes_count();

      if (set_layout_count > 0)
      {
        pipeline_layout_info.setLayoutCount = set_layout_count;
        pipeline_layout_info.pSetLayouts    = m_uniform_data_storage->get_layouts_data();
      }
      if (pushes_count > 0)
      {
        pipeline_layout_info.pushConstantRangeCount = pushes_count;
        pipeline_layout_info.pPushConstantRanges    = m_uniform_data_storage->get_push_data();
      }
    }
    else { m_uniform_data_storage = nullptr; }

    if (vkCreatePipelineLayout(VulkanDevice::get_logical_device(),
                               &pipeline_layout_info,
                               nullptr,
                               &m_pipeline_layout) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create graphics pipeline");
      throw std::runtime_error("Failed to create graphics pipeline");
    }

    m_is_pipeline_layout = true;
  }

  std::unique_ptr<EspWorker> VulkanWorkerBuilder::build_worker()
  {
    /*
      DEFAULT INFO ABOUT PIPELINE:
      (0) Shaders: it is changeable.

      (1) Vertex input state: it is changeable.

      (2) Topology: pipeline uses triangle list.

      (3) Viewport and Scissors: pipeline anables to use only 1 viewport and scissors which
      are dynamic.

      (4) Rasterization:
        - the tirable is filled with color,
        - the width of line is 1,
        - back triangles are culled and front face has clockwise order.

      (5) Multisampling: there is no multisampling.

      (6) Depth stencil: less operation for comparing.

      (7) Blending: it is off.

      (8) Pipeline layout: it is changeable.

      (9) Render pass and subpass:
        - the render pass is taken from VulkanFrameManager
        - there is 0 subpasses.
    */
    ESP_ASSERT(m_is_fragment_shader_module, "You cannot create pipeline a without a fragment shader.");
    ESP_ASSERT(m_is_vertex_shader_module, "You cannot create pipeline a without a vertex shader.");
    ESP_ASSERT(m_is_pipeline_layout, "You cannot create a pipeline without a pipeline layout.")
    ESP_ASSERT(m_color_attachment_formats.size() != 0, "You cannot create a pipeline  without color attachments.");

    VkPipelineShaderStageCreateInfo shader_stages[] = { m_vertex_shader_info, m_fragment_shader_info };

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    {
      vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    }

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    {
      input_assembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      input_assembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      input_assembly.primitiveRestartEnable = VK_FALSE;
    }

    VkPipelineViewportStateCreateInfo viewport_state{};
    {
      viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewport_state.viewportCount = 1;
      viewport_state.scissorCount  = 1;
    }

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    {
      rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizer.depthClampEnable        = VK_FALSE;
      rasterizer.rasterizerDiscardEnable = VK_FALSE;
      rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
      rasterizer.lineWidth               = 1.0f;
      rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
      rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
      rasterizer.depthBiasEnable         = VK_FALSE;
    }

    VkPipelineMultisampleStateCreateInfo multisampling{};
    {
      multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multisampling.sampleShadingEnable  = VK_FALSE;
      multisampling.rasterizationSamples = static_cast<VkSampleCountFlagBits>(m_multisampling.m_sample_count_flag);
      //
      // TODO: let user decide whether he wants higher quality or better performance - put this in some if statement
      // multisampling.sampleShadingEnable  = VK_TRUE; // enable sample shading in the pipeline
      // multisampling.minSampleShading     = .2f;     // min fraction for sample shading; closer to one is smoother
      //
    }

    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    VkPipelineDepthStencilStateCreateInfo* p_depth_stencil = nullptr;
    if (m_depth_test.m_enable)
    {
      depth_stencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depth_stencil.depthTestEnable       = VK_TRUE;
      depth_stencil.depthWriteEnable      = VK_TRUE;
      depth_stencil.depthCompareOp        = static_cast<VkCompareOp>(m_depth_test.m_compare_op);
      depth_stencil.depthBoundsTestEnable = VK_FALSE;
      depth_stencil.stencilTestEnable     = VK_FALSE;

      p_depth_stencil = &depth_stencil;
    }

    VkPipelineColorBlendStateCreateInfo color_blending{};
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    {
      color_blend_attachment.colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
      color_blend_attachment.blendEnable = VK_FALSE;

      color_blending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      color_blending.logicOpEnable     = VK_FALSE;
      color_blending.logicOp           = VK_LOGIC_OP_COPY;
      color_blending.attachmentCount   = 1;
      color_blending.pAttachments      = &color_blend_attachment;
      color_blending.blendConstants[0] = 0.0f;
      color_blending.blendConstants[1] = 0.0f;
      color_blending.blendConstants[2] = 0.0f;
      color_blending.blendConstants[3] = 0.0f;
    }

    VkPipelineDynamicStateCreateInfo dynamic_state{};
    std::vector<VkDynamicState> dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    {
      dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
      dynamic_state.pDynamicStates    = dynamic_states.data();
    }

    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info{};
    {
      pipeline_rendering_create_info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
      pipeline_rendering_create_info.colorAttachmentCount    = static_cast<uint32_t>(m_color_attachment_formats.size());
      pipeline_rendering_create_info.pColorAttachmentFormats = m_color_attachment_formats.data();
      if (m_depth_test.m_enable)
      {
        pipeline_rendering_create_info.depthAttachmentFormat = static_cast<VkFormat>(m_depth_test.m_format);
      }
    }

    /* ----- GRAPHICS PIPELINE CREATE INFO ------ */
    /* ------------------------------------------ */
    VkGraphicsPipelineCreateInfo pipeline_info{};

    pipeline_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount          = 2;
    pipeline_info.pStages             = shader_stages;
    pipeline_info.pVertexInputState   = &m_vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState      = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState   = &multisampling;
    pipeline_info.pDepthStencilState  = p_depth_stencil;
    pipeline_info.pColorBlendState    = &color_blending;
    pipeline_info.pDynamicState       = &dynamic_state;
    pipeline_info.layout              = m_pipeline_layout;
    // !!! IT IS NOT NEEDED ANYMORE !!!! DUE TO DYNAMIC RENDERING ....
    // pipeline_info.renderPass          = VulkanFrameManager::get_swap_chain_render_pass();
    pipeline_info.subpass            = 0;
    pipeline_info.basePipelineIndex  = -1;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.pNext              = &pipeline_rendering_create_info;

    VkPipeline graphics_pipeline;

    if (vkCreateGraphicsPipelines(VulkanDevice::get_logical_device(),
                                  VK_NULL_HANDLE,
                                  1,
                                  &pipeline_info,
                                  nullptr,
                                  &graphics_pipeline) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create graphics pipeline!\n");
    }
    else { ESP_CORE_INFO("Graphic pipeline created correctly"); }

    vkDestroyShaderModule(VulkanDevice::get_logical_device(), m_fragment_shader_module, nullptr);
    vkDestroyShaderModule(VulkanDevice::get_logical_device(), m_vertex_shader_module, nullptr);

    m_is_fragment_shader_module = false;
    m_is_vertex_shader_module   = false;
    m_is_pipeline_layout        = false;

    return std::unique_ptr<EspWorker>{
      new VulkanWorker(m_pipeline_layout, graphics_pipeline, std::move(m_uniform_data_storage))
    };
  }
} // namespace esp

/* --------------------------------------------------------- */
/* ------------------ HELPFUL FUNCTIONS -------------------- */
/* --------------------------------------------------------- */

static std::vector<char> read_file(const std::string& filename)
{
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open())
  {
    ESP_CORE_ERROR("The file doesn't exist: {0}", filename);
    throw std::runtime_error("The file doesn't exist.");
  }

  size_t file_size = (size_t)file.tellg();
  std::vector<char> buffer(file_size);

  file.seekg(0);
  file.read(buffer.data(), file_size);

  file.close();

  return buffer;
}

static VkShaderModule create_shader_module(const std::vector<char>& code, VkDevice device)
{
  VkShaderModuleCreateInfo create_info{};
  create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode    = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shader_module;
  if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
  {
    ESP_CORE_ERROR("Failed to create shader module");
    throw std::runtime_error("Failed to create shader module");
  }

  return shader_module;
}
