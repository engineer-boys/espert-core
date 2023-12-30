#include "Platform/Vulkan/Resources/VulkanShader.hh"
#include "Core/RenderAPI/Pipeline/EspAttrFormat.hh"
#include "Core/RenderAPI/Resources/EspShader.hh"
#include "Platform/Vulkan/Pipeline/VulkanPipelineBuilder.hh"

static uint32_t format_to_size(SpvReflectFormat format);

namespace esp
{
  VulkanShader::VulkanShader(const std::string& name, std::unique_ptr<EspWorker> worker) :
      EspShader(name, std::move(worker))
  {
  }

  std::shared_ptr<VulkanShader> VulkanShader::create(const std::string name,
                                                     std::unique_ptr<SpirvResource> spirv_resource,
                                                     const EspShaderConfig& config)
  {
    auto builder = VulkanWorkerBuilder::create();

    if (config.depthtest_config.enable)
    {
      builder->enable_depth_test(config.depthtest_config.format, config.depthtest_config.compare_op);
    }

    if (config.multisampling_config.enable)
    {
      builder->enable_multisampling(config.multisampling_config.sample_count_flag);
    }

    if (!config.color_attachement_formats.empty())
    {
      builder->set_attachment_formats(std::move(config.color_attachement_formats));
    }

    {
      auto shader_module_map = VulkanShader::generate_shader_modules(*spirv_resource);

      auto vertex_layouts = VulkanShader::generate_vertex_layouts(shader_module_map, config.vertex_input_config);

      builder->set_vertex_layouts(std::move(vertex_layouts));

      auto uniform_metadata = VulkanShader::generate_uniform_metadata(shader_module_map);

      builder->set_worker_layout(std::move(uniform_metadata));
    }

    builder->set_shaders(std::move(spirv_resource));

    builder->set_specialization(config.spec_const_map);

    return std::shared_ptr<VulkanShader>(new VulkanShader(name, std::move(builder->build_worker())));
  }

  VulkanShader::~VulkanShader() {}

  std::unique_ptr<EspUniformMetaData> VulkanShader::generate_uniform_metadata(const ShaderModuleMap& shader_module_map)
  {
    auto uniform_meta_data = EspUniformMetaData::create();

    std::map<uint32_t, std::unordered_map<EspShaderStage, SpvReflectDescriptorSet*>> descriptor_set_map = {};

    for (const auto& [stage, shader_module] : shader_module_map)
    {
      uint32_t count = 0;
      shader_module.EnumerateDescriptorSets(&count, nullptr);
      CHECK_SPV_REFL(shader_module.GetResult(), stage);
      std::vector<SpvReflectDescriptorSet*> sets(count);
      shader_module.EnumerateDescriptorSets(&count, sets.data());
      CHECK_SPV_REFL(shader_module.GetResult(), stage);
      for (auto set : sets)
      {
        descriptor_set_map[set->set].insert({ stage, set });
        // descriptor_set_map.insert({ set->set, { stage, set } });
      }

      shader_module.EnumeratePushConstantBlocks(&count, NULL);
      CHECK_SPV_REFL(shader_module.GetResult(), stage);
      std::vector<SpvReflectBlockVariable*> push_constant(count);
      shader_module.EnumeratePushConstantBlocks(&count, push_constant.data());

      for (const auto& refl_push : push_constant)
      {
        uniform_meta_data->add_push_uniform(stage, refl_push->offset, refl_push->size);
      }
    }

    for (const auto& [set_i, stage_set_map] : descriptor_set_map)
    {
      uniform_meta_data->establish_descriptor_set();

      std::map<uint32_t, std::pair<EspShaderStage, const SpvReflectDescriptorBinding&>> bindings = {};

      for (const auto& [stage, refl_set] : stage_set_map)
      {
        for (uint32_t binding = 0; binding < refl_set->binding_count; ++binding)
        {
          const auto& refl_binding = *(refl_set->bindings[binding]);
          bindings.insert({ refl_binding.binding, { stage, refl_binding } });
        }
      }

      for (const auto& [binding_i, stage_refl] : bindings)
      {
        auto stage               = stage_refl.first;
        const auto& refl_binding = stage_refl.second;

        if (refl_binding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
        {
          uniform_meta_data->add_texture_uniform(stage, refl_binding.count);
        }
        else if (refl_binding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
        {
          uniform_meta_data->add_buffer_uniform(stage, refl_binding.block.size, refl_binding.count);
        }
      }
    }

    if (descriptor_set_map.empty()) { uniform_meta_data->establish_descriptor_set(); }

    return uniform_meta_data;
  }

  std::vector<EspVertexLayout> VulkanShader::generate_vertex_layouts(
      const ShaderModuleMap& shader_module_map,
      const std::vector<EspShaderVertexInputConfig>& config)
  {
    const auto& shader_module = shader_module_map.at(EspShaderStage::VERTEX);

    uint32_t count = 0;
    shader_module.EnumerateInputVariables(&count, nullptr);
    CHECK_SPV_REFL(shader_module.GetResult(), EspShaderStage::VERTEX);
    std::vector<SpvReflectInterfaceVariable*> input_vars(count);
    shader_module.EnumerateInputVariables(&count, input_vars.data());
    CHECK_SPV_REFL(shader_module.GetResult(), EspShaderStage::VERTEX);
    std::map<uint32_t, SpvReflectInterfaceVariable*> sorted_input_vars;

    for (const auto& input_var : input_vars)
    {
      sorted_input_vars.insert({ input_var->location, input_var });
    }

    std::vector<EspVertexLayout> vertex_layouts;

    if (config.empty())
    {
      uint32_t size = 0;
      std::vector<EspVertexAttribute> vertex_attributes;

      for (const auto& [location, refl_input] : sorted_input_vars)
      {
        vertex_attributes.push_back({ refl_input->location, spv_to_esp_format(refl_input->format), size });
        size += format_to_size(refl_input->format);
      }

      EspVertexLayout vertex_layout = VTX_LAYOUT(size, 0, ESP_VERTEX_INPUT_RATE_VERTEX, {});
      vertex_layout.m_attrs.swap(vertex_attributes);

      vertex_layouts.push_back(std::move(vertex_layout));
    }
    else
    {
      std::unordered_map<uint32_t, EspShaderVertexInputConfig> attr_config_map;

      for (const auto& attr : config)
      {
        attr_config_map.insert({ attr.location, attr });
      }

      for (const auto& [location, refl_input] : sorted_input_vars)
      {
        if (attr_config_map[refl_input->location].binding + 1 > vertex_layouts.size())
        {
          EspVertexLayout vertex_layout = VTX_LAYOUT(0,
                                                     attr_config_map[refl_input->location].binding,
                                                     attr_config_map[refl_input->location].input_rate,
                                                     {});
          vertex_layouts.push_back(std::move(vertex_layout));
        }

        if (refl_input->numeric.matrix.row_count > 0)
        {
          for (int i = 0; i < refl_input->numeric.matrix.row_count; ++i)
          {
            vertex_layouts[attr_config_map[refl_input->location].binding].m_attrs.push_back(
                { refl_input->location + i,
                  spv_to_esp_format(refl_input->format),
                  vertex_layouts[attr_config_map[refl_input->location].binding].m_size });
            vertex_layouts[attr_config_map[refl_input->location].binding].m_size += format_to_size(refl_input->format);
          }
        }
        else
        {
          vertex_layouts[attr_config_map[refl_input->location].binding].m_attrs.push_back(
              { refl_input->location,
                spv_to_esp_format(refl_input->format),
                vertex_layouts[attr_config_map[refl_input->location].binding].m_size });
          vertex_layouts[attr_config_map[refl_input->location].binding].m_size += format_to_size(refl_input->format);
        }
      }
    }

    return vertex_layouts;
  }

  ShaderModuleMap VulkanShader::generate_shader_modules(const SpirvResource& spirv_resource)
  {
    ShaderModuleMap shader_module_map = {};

    spirv_resource.enumerate_data(
        [&shader_module_map](EspShaderStage stage, const SpirvData& spirv_data) {
          shader_module_map.insert({ stage, spv_reflect::ShaderModule(spirv_data, SPV_REFLECT_MODULE_FLAG_NO_COPY) });
        });

    return shader_module_map;
  }
} // namespace esp

static uint32_t format_to_size(SpvReflectFormat format)
{
  uint32_t result = 0;
  switch (format)
  {
  case SPV_REFLECT_FORMAT_UNDEFINED:
    result = 0;
    break;
  case SPV_REFLECT_FORMAT_R16_UINT:
    result = 2;
    break;
  case SPV_REFLECT_FORMAT_R16_SINT:
    result = 2;
    break;
  case SPV_REFLECT_FORMAT_R16_SFLOAT:
    result = 2;
    break;
  case SPV_REFLECT_FORMAT_R16G16_UINT:
    result = 4;
    break;
  case SPV_REFLECT_FORMAT_R16G16_SINT:
    result = 4;
    break;
  case SPV_REFLECT_FORMAT_R16G16_SFLOAT:
    result = 4;
    break;
  case SPV_REFLECT_FORMAT_R16G16B16_UINT:
    result = 6;
    break;
  case SPV_REFLECT_FORMAT_R16G16B16_SINT:
    result = 6;
    break;
  case SPV_REFLECT_FORMAT_R16G16B16_SFLOAT:
    result = 6;
    break;
  case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:
    result = 8;
    break;
  case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:
    result = 8;
    break;
  case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:
    result = 8;
    break;
  case SPV_REFLECT_FORMAT_R32_UINT:
    result = 4;
    break;
  case SPV_REFLECT_FORMAT_R32_SINT:
    result = 4;
    break;
  case SPV_REFLECT_FORMAT_R32_SFLOAT:
    result = 4;
    break;
  case SPV_REFLECT_FORMAT_R32G32_UINT:
    result = 8;
    break;
  case SPV_REFLECT_FORMAT_R32G32_SINT:
    result = 8;
    break;
  case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
    result = 8;
    break;
  case SPV_REFLECT_FORMAT_R32G32B32_UINT:
    result = 12;
    break;
  case SPV_REFLECT_FORMAT_R32G32B32_SINT:
    result = 12;
    break;
  case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
    result = 12;
    break;
  case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
    result = 16;
    break;
  case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
    result = 16;
    break;
  case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
    result = 16;
    break;
  case SPV_REFLECT_FORMAT_R64_UINT:
    result = 8;
    break;
  case SPV_REFLECT_FORMAT_R64_SINT:
    result = 8;
    break;
  case SPV_REFLECT_FORMAT_R64_SFLOAT:
    result = 8;
    break;
  case SPV_REFLECT_FORMAT_R64G64_UINT:
    result = 16;
    break;
  case SPV_REFLECT_FORMAT_R64G64_SINT:
    result = 16;
    break;
  case SPV_REFLECT_FORMAT_R64G64_SFLOAT:
    result = 16;
    break;
  case SPV_REFLECT_FORMAT_R64G64B64_UINT:
    result = 24;
    break;
  case SPV_REFLECT_FORMAT_R64G64B64_SINT:
    result = 24;
    break;
  case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:
    result = 24;
    break;
  case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:
    result = 32;
    break;
  case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:
    result = 32;
    break;
  case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:
    result = 32;
    break;
  default:
    break;
  }
  return result;
}
