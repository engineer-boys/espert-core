#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_HH

// Render API
#include "Core/RenderAPI/Uniforms/EspUniformMetaData.hh"

#include "esppch.hh"

namespace esp
{
  struct EspMetaDescriptorSet
  {
   public:
    std::vector<ShaderProperty> m_shader_properties;

    uint32_t m_set_index;

   public:
    EspMetaDescriptorSet(uint32_t set_index);
    EspMetaDescriptorSet(uint32_t set_index, ShaderProperty property);
    EspMetaDescriptorSet(uint32_t set_index, std::vector<ShaderProperty> properties);

    inline void push_back(ShaderProperty& property) { m_shader_properties.push_back(property); }
    template<ShaderPropertyType type = ShaderPropertyType::All> inline int count()
    {
      return std::count_if(m_shader_properties.begin(),
                           m_shader_properties.end(),
                           [](const auto& prop) { prop.type == type; });
    }
    inline std::vector<ShaderProperty>::iterator begin() { return m_shader_properties.begin(); }
    inline std::vector<ShaderProperty>::iterator end() { return m_shader_properties.end(); }
    inline std::vector<ShaderProperty>::const_iterator begin() const { return m_shader_properties.begin(); }
    inline std::vector<ShaderProperty>::const_iterator end() const { return m_shader_properties.end(); }
  };

  struct VulkanUniformMetaData : public EspUniformMetaData
  {
   public:
    std::unordered_map<ShaderPropertyType, uint32_t> m_shader_property_counter;
    std::unordered_map<uint32_t, EspMetaDescriptorSet> m_meta_descriptor_sets;
    std::vector<VkPushConstantRange> m_push_constant_ranges;

   public:
    operator bool() const;

   public:
    VulkanUniformMetaData();
    virtual ~VulkanUniformMetaData();

    virtual EspUniformMetaData& add_shader_stage_property(ShaderStage stage, ShaderProperty shader_property) override;

    virtual EspUniformMetaData& load_shader_stage_data(ShaderStage stage, std::vector<uint32_t> source) override;

    virtual EspUniformMetaData& establish_descriptor_set() override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_HH */