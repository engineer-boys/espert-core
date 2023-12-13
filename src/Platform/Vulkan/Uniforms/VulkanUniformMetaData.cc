#include "VulkanUniformMetaData.hh"

#include "Core/RenderAPI/Resources/SpirvReflection.hh"

namespace esp
{
  EspMetaDescriptorSet::EspMetaDescriptorSet(uint32_t set_index) : m_set_index{ set_index } {}

  EspMetaDescriptorSet::EspMetaDescriptorSet(uint32_t set_index, std::vector<ShaderProperty> properties) :
      EspMetaDescriptorSet(set_index)
  {
    m_shader_properties = properties;
  }

  EspMetaDescriptorSet::EspMetaDescriptorSet(uint32_t set_index, ShaderProperty property) :
      EspMetaDescriptorSet(set_index)
  {
    m_shader_properties = std::vector<ShaderProperty>();
    m_shader_properties.push_back(property);
  }

  VulkanUniformMetaData::VulkanUniformMetaData() {}
  VulkanUniformMetaData::~VulkanUniformMetaData() {}

  EspUniformMetaData& VulkanUniformMetaData::add_shader_stage_property(ShaderStage stage,
                                                                       ShaderProperty shader_property)
  {
    if (!m_shader_properties.contains(stage)) { m_shader_properties[stage] = std::vector<ShaderProperty>(); }

    m_shader_properties.at(stage).push_back(shader_property);
    m_shader_property_counter[shader_property.type]++;

    return *this;
  }

  EspUniformMetaData& VulkanUniformMetaData::load_shader_stage_data(ShaderStage stage, std::vector<uint32_t> source)
  {
    if (m_shader_properties.contains(stage))
    {
      ESP_CORE_WARN("Uniform metadata already contains metadata for shader stage {}.", static_cast<uint8_t>(stage));
      return *this;
    }

    spirv_cross::Compiler reflection_object(source);
    std::vector<ShaderProperty> properties;
    SpirvReflection::parse_shader_properties(reflection_object, stage, properties);
    for (const auto& prop : properties)
    {
      m_shader_property_counter[prop.type]++;
    }
    m_shader_properties[stage] = std::move(properties);

    return *this;
  }

  EspUniformMetaData& VulkanUniformMetaData::establish_descriptor_set()
  {
    std::unordered_map<std::string, ShaderProperty> pipeline_shader_properties;

    for (const auto& shader_properties : m_shader_properties)
    {
      for (const auto& shader_property : shader_properties.second)
      {
        std::string key = shader_property.name;

        if (shader_property.type == ShaderPropertyType::Input || shader_property.type == ShaderPropertyType::Output)
        {
          key = std::to_string(shader_property.stages) + "_" + key;
        }

        auto it = pipeline_shader_properties.find(key);

        if (it != pipeline_shader_properties.end()) { it->second.stages |= shader_property.stages; }
        else { pipeline_shader_properties.emplace(key, shader_property); }
      }
    }

    for (auto& it : pipeline_shader_properties)
    {
      auto& shader_property = it.second;

      auto it2 = m_meta_descriptor_sets.find(shader_property.set);

      if (it2 != m_meta_descriptor_sets.end()) { it2->second.push_back(shader_property); }
      else
      {
        m_meta_descriptor_sets.emplace(shader_property.set,
                                       EspMetaDescriptorSet{ shader_property.set, shader_property });
      }
    }

    for (auto& shader_property : pipeline_shader_properties)
    {
      if (shader_property.second.type == ShaderPropertyType::PushConstant)
      {
        auto& push_constant_property = shader_property.second;
        m_push_constant_ranges.push_back(
            { push_constant_property.stages, push_constant_property.offset, push_constant_property.size });
      }
    }

    return *this;
  }

  VulkanUniformMetaData::operator bool() const { return !m_shader_property_counter.empty(); }
} // namespace esp
