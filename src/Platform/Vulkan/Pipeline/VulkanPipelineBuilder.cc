#include "VulkanPipelineBuilder.hh"

// std
#include <fstream>

// Platform
#include "Platform/Vulkan/Uniforms/VulkanUniformMetaData.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanFrameManager.hh"
#include "VulkanPipeline.hh"

// signatures
static std::vector<char> read_file(const std::string& filename);
static VkShaderModule create_shader_module(const std::vector<char>& code, VkDevice device);

/* --------------------------------------------------------- */
/* ---------------- CLASS IMPLEMENTATION ------------------- */
/* --------------------------------------------------------- */

namespace esp
{
  VulaknPipelineBuilder::VulaknPipelineBuilder() :
      m_shader_modules{}, m_pipeline_shader_info{}, m_vertex_input_info{}, m_descriptor_set_layouts{},
      m_push_constant_ranges{}
  {
  }

  VulaknPipelineBuilder::~VulaknPipelineBuilder()
  {
    if (!m_shader_modules.empty())
    {
      for (auto& shader_module : m_shader_modules)
      {
        vkDestroyShaderModule(VulkanDevice::get_logical_device(), shader_module.second, nullptr);
      }

      std::unordered_map<ShaderStage, VkShaderModule> shader_modules_empty;
      m_shader_modules.swap(shader_modules_empty);

      std::unordered_map<ShaderStage, VkPipelineShaderStageCreateInfo> pipeline_shader_info_empty;
      m_pipeline_shader_info.swap(pipeline_shader_info_empty);
    }

    if (m_is_pipeline_layout)
    {
      vkDestroyPipelineLayout(VulkanDevice::get_logical_device(), m_pipeline_layout, nullptr);
    }

    if (!m_descriptor_set_layouts.empty())
    {
      for (auto& descriptor_set_layout : m_descriptor_set_layouts)
      {
        vkDestroyDescriptorSetLayout(VulkanDevice::get_logical_device(), descriptor_set_layout, nullptr);
      }
    }
  }

  void VulaknPipelineBuilder::set_shader(ShaderStage stage, std::vector<uint32_t> source)
  {
    VkShaderModule shader_module = create_shader_module(source, VulkanDevice::get_logical_device());
    VkPipelineShaderStageCreateInfo pipeline_shader_info;

    pipeline_shader_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_shader_info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    pipeline_shader_info.module = shader_module;
    pipeline_shader_info.pName  = "main";

    m_shader_modules.insert({ stage, shader_module });
    m_pipeline_shader_info.insert({ stage, pipeline_shader_info });
  }

  void VulaknPipelineBuilder::set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts)
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

  void VulaknPipelineBuilder::set_pipeline_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data)
  {
    std::unique_ptr<VulkanUniformMetaData> meta_data(static_cast<VulkanUniformMetaData*>(uniforms_meta_data.release()));

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if (*meta_data)
    {
      m_uniform_data_storage                      = std::make_unique<EspUniformDataStorage>(std::move(meta_data));
      pipeline_layout_info.setLayoutCount         = m_uniform_data_storage->get_layouts_number();
      pipeline_layout_info.pSetLayouts            = m_uniform_data_storage->get_layouts_data();
      pipeline_layout_info.pushConstantRangeCount = m_uniform_data_storage->get_push_constants_number();
      pipeline_layout_info.pPushConstantRanges    = m_uniform_data_storage->get_push_constant_data();
    }

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

  //   void VulaknPipelineBuilder::set_pipeline_layout(
  //       std::unordered_map<ShaderStage, std::vector<ShaderProperty>> shader_properties_map)
  //   {
  //     std::unordered_map<std::string, ShaderProperty> pipeline_shader_properties;

  //     for (const auto& shader_properties : shader_properties_map)
  //     {
  //       for (const auto& shader_property : shader_properties.second)
  //       {
  //         std::string key = shader_property.name;

  //         if (shader_property.type == ShaderPropertyType::Input || shader_property.type ==
  //         ShaderPropertyType::Output)
  //         {
  //           key = std::to_string(shader_property.stages) + "_" + key;
  //         }

  //         auto it = pipeline_shader_properties.find(key);

  //         if (it != pipeline_shader_properties.end()) { it->second.stages |= shader_property.stages; }
  //         else { pipeline_shader_properties.emplace(key, shader_property); }
  //       }
  //     }

  //     std::unordered_map<uint32_t, std::vector<ShaderProperty>> shader_sets;

  //     for (auto& it : pipeline_shader_properties)
  //     {
  //       auto& shader_property = it.second;

  //       auto it2 = shader_sets.find(shader_property.set);

  //       if (it2 != shader_sets.end()) { it2->second.push_back(shader_property); }
  //       else { shader_sets.emplace(shader_property.set, std::vector<ShaderProperty>{ shader_property }); }
  //     }

  //     for (auto& shader_set_it : shader_sets)
  //     {
  //       m_descriptor_set_layouts.emplace_back(create_descriptor_set_layout(shader_set_it.first,
  //       shader_set_it.second));
  //     }

  //     for (auto& shader_property : pipeline_shader_properties)
  //     {
  //       if (shader_property.second.type == ShaderPropertyType::PushConstant)
  //       {
  //         auto& push_constant_property = shader_property.second;
  //         m_push_constant_ranges.push_back(
  //             { push_constant_property.stages, push_constant_property.offset, push_constant_property.size });
  //       }
  //     }

  //     VkPipelineLayoutCreateInfo create_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

  //     create_info.setLayoutCount         = static_cast<uint32_t>(m_descriptor_set_layouts.size());
  //     create_info.pSetLayouts            = m_descriptor_set_layouts.data();
  //     create_info.pushConstantRangeCount = static_cast<uint32_t>(m_push_constant_ranges.size());
  //     create_info.pPushConstantRanges    = m_push_constant_ranges.data();

  //     if (vkCreatePipelineLayout(VulkanDevice::get_logical_device(), &create_info, nullptr, &m_pipeline_layout) !=
  //         VK_SUCCESS)
  //     {
  //       ESP_CORE_ERROR("Failed to create graphics pipeline");
  //       throw std::runtime_error("Failed to create graphics pipeline");
  //     }

  //     m_is_pipeline_layout = true;
  //   }

  std::unique_ptr<EspPipeline> VulaknPipelineBuilder::build_pipeline()
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
    ESP_ASSERT(m_is_pipeline_layout, "You cannot create a pipeline without a pipeline layout.")

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
      multisampling.rasterizationSamples = VulkanContext::get_context_data().m_msaa_samples;
      //
      // TODO: let user decide whether he wants higher quality or better performance - put this in some if statement
      // multisampling.sampleShadingEnable  = VK_TRUE; // enable sample shading in the pipeline
      // multisampling.minSampleShading     = .2f;     // min fraction for sample shading; closer to one is smoother
      //
    }

    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    {
      depth_stencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depth_stencil.depthTestEnable       = VK_TRUE;
      depth_stencil.depthWriteEnable      = VK_TRUE;
      depth_stencil.depthCompareOp        = VK_COMPARE_OP_LESS;
      depth_stencil.depthBoundsTestEnable = VK_FALSE;
      depth_stencil.stencilTestEnable     = VK_FALSE;
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

    /* ----- GRAPHICS PIPELINE CREATE INFO ------ */
    /* ------------------------------------------ */
    VkGraphicsPipelineCreateInfo pipeline_info{};

    VkPipelineShaderStageCreateInfo* shader_stages = new VkPipelineShaderStageCreateInfo[m_pipeline_shader_info.size()];
    int i                                          = 0;
    for (auto& it : m_pipeline_shader_info)
    {
      shader_stages[i++] = it.second;
    }

    pipeline_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount          = 2;
    pipeline_info.pStages             = shader_stages;
    pipeline_info.pVertexInputState   = &m_vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState      = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState   = &multisampling;
    pipeline_info.pDepthStencilState  = &depth_stencil;
    pipeline_info.pColorBlendState    = &color_blending;
    pipeline_info.pDynamicState       = &dynamic_state;
    pipeline_info.layout              = m_pipeline_layout;
    pipeline_info.renderPass          = VulkanFrameManager::get_swap_chain_render_pass();
    pipeline_info.subpass             = 0;
    pipeline_info.basePipelineIndex   = -1;
    pipeline_info.basePipelineHandle  = VK_NULL_HANDLE;

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

    for (auto& shader_module : m_shader_modules)
    {
      vkDestroyShaderModule(VulkanDevice::get_logical_device(), shader_module.second, nullptr);
    }

    std::unordered_map<ShaderStage, VkShaderModule> shader_modules_empty;
    m_shader_modules.swap(shader_modules_empty);

    std::unordered_map<ShaderStage, VkPipelineShaderStageCreateInfo> pipeline_shader_info_empty;
    m_pipeline_shader_info.swap(pipeline_shader_info_empty);

    m_is_pipeline_layout = false;

    return std::unique_ptr<EspPipeline>{
      new VulkanPipeline(m_pipeline_layout, graphics_pipeline, std::move(m_uniform_data_storage))
    };
  }
} // namespace esp

/* --------------------------------------------------------- */
/* ------------------ HELPFUL FUNCTIONS -------------------- */
/* --------------------------------------------------------- */

static VkShaderModule create_shader_module(const std::vector<uint32_t>& code, VkDevice device)
{
  VkShaderModuleCreateInfo create_info{};
  create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size() * sizeof(uint32_t);
  create_info.pCode    = code.data();

  VkShaderModule shader_module;
  if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
  {
    ESP_CORE_ERROR("Failed to create shader module");
    throw std::runtime_error("Failed to create shader module");
  }

  return shader_module;
}

// static VkDescriptorType find_descriptor_type(esp::ShaderPropertyType property_type, bool dynamic)
// {
//   switch (property_type)
//   {
//   case esp::ShaderPropertyType::InputAttachment:
//     return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
//     break;
//   case esp::ShaderPropertyType::Image:
//     return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
//     break;
//   case esp::ShaderPropertyType::ImageSampler:
//     return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//     break;
//   case esp::ShaderPropertyType::ImageStorage:
//     return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
//     break;
//   case esp::ShaderPropertyType::Sampler:
//     return VK_DESCRIPTOR_TYPE_SAMPLER;
//     break;
//   case esp::ShaderPropertyType::BufferUniform:
//     if (dynamic) { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; }
//     else { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
//     break;
//   case esp::ShaderPropertyType::BufferStorage:
//     if (dynamic) { return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC; }
//     else { return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; }
//     break;
//   default:
//     throw std::runtime_error("No conversion possible for the shader resource type.");
//     break;
//   }
// }

// static bool validate_binding(const VkDescriptorSetLayoutBinding& binding,
//                              const std::vector<VkDescriptorType>& blacklist)
// {
//   return !(std::find_if(blacklist.begin(),
//                         blacklist.end(),
//                         [binding](const VkDescriptorType& type)
//                         { return type == binding.descriptorType; }) != blacklist.end());
// }

// static bool validate_flags(const std::vector<VkDescriptorSetLayoutBinding>& bindings,
//                            const std::vector<VkDescriptorBindingFlagsEXT>& flags)
// {
//   if (flags.empty()) { return true; }

//   if (bindings.size() != flags.size())
//   {
//     ESP_CORE_ERROR("Binding count has to be equal to flag count.");
//     return false;
//   }

//   return true;
// }

// static VkDescriptorSetLayout create_descriptor_set_layout(const uint32_t set_index,
//                                                           const std::vector<esp::ShaderProperty>& property_set)
// {
//   std::vector<VkDescriptorSetLayoutBinding> bindings;
//   std::vector<VkDescriptorBindingFlagsEXT> binding_flags;

//   for (auto& property : property_set)
//   {
//     if (property.type == esp::ShaderPropertyType::Input || property.type == esp::ShaderPropertyType::Output ||
//         property.type == esp::ShaderPropertyType::PushConstant ||
//         property.type == esp::ShaderPropertyType::SpecializationConstant)
//     {
//       continue;
//     }

//     auto descriptor_type = find_descriptor_type(property.type, property.mode == esp::ShaderPropertyMode::Dynamic);

//     if (property.mode == esp::ShaderPropertyMode::UpdateAfterBind)
//     {
//       binding_flags.push_back(VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT);
//     }
//     else { binding_flags.push_back(0); }

//     VkDescriptorSetLayoutBinding layout_binding{};

//     layout_binding.binding         = property.binding;
//     layout_binding.descriptorCount = property.array_size;
//     layout_binding.descriptorType  = descriptor_type;
//     layout_binding.stageFlags      = static_cast<VkShaderStageFlags>(property.stages);

//     bindings.push_back(layout_binding);
//   }

//   VkDescriptorSetLayoutCreateInfo create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
//   create_info.flags        = 0;
//   create_info.bindingCount = static_cast<uint32_t>(bindings.size());
//   create_info.pBindings    = bindings.data();

//   if (std::find_if(property_set.begin(),
//                    property_set.end(),
//                    [](const esp::ShaderProperty& shader_property)
//                    { return shader_property.mode == esp::ShaderPropertyMode::UpdateAfterBind; }) !=
//                    property_set.end())
//   {
//     if (std::find_if(property_set.begin(),
//                      property_set.end(),
//                      [](const esp::ShaderProperty& shader_property)
//                      { return shader_property.mode == esp::ShaderPropertyMode::Dynamic; }) != property_set.end())
//     {
//       throw std::runtime_error("Cannot create descriptor set layout, dynamic resources are not allowed if at least
//       one "
//                                "resource is update-after-bind.");
//     }

//     if (!validate_flags(bindings, binding_flags))
//     {
//       throw std::runtime_error("Invalid binding, couldn't create descriptor set layout.");
//     }

//     VkDescriptorSetLayoutBindingFlagsCreateInfoEXT binding_flags_create_info{
//       VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT
//     };
//     binding_flags_create_info.bindingCount  = static_cast<uint32_t>(binding_flags.size());
//     binding_flags_create_info.pBindingFlags = binding_flags.data();

//     create_info.pNext = &binding_flags_create_info;
//     create_info.flags |=
//         std::find(binding_flags.begin(), binding_flags.end(), VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT) !=
//             binding_flags.end()
//         ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT
//         : 0;
//   }

//   VkDescriptorSetLayout descriptor_set_layout;
//   if (vkCreateDescriptorSetLayout(esp::VulkanDevice::get_logical_device(),
//                                   &create_info,
//                                   nullptr,
//                                   &descriptor_set_layout) != VK_SUCCESS)
//   {
//     ESP_CORE_ERROR("Failed to create desscriptor set layout.");
//     throw std::runtime_error("Failed to create desscriptor set layout.");
//   }

//   return descriptor_set_layout;
// }
