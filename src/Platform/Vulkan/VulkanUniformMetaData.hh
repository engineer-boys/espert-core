#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_HH

// Render API
#include "Core/RenderAPI/EspUniformMetaData.hh"

// libs
#include "volk.h"

namespace esp
{
  struct VulkanMetaUniform
  {
    EspUniformShaderStage m_stage;

    // Size of basic data type.
    uint32_t m_size_of_data_chunk;

    // Number of textures or objects for uniform.
    uint32_t m_number_of_elements;

    uint32_t m_binding;

    EspUniformType m_uniform_type;

    VulkanMetaUniform(EspUniformShaderStage stage,
                      uint32_t size_of_data_chunk,
                      uint32_t count_of_elements,
                      uint32_t binding,
                      EspUniformType uniform_type);
  };

  struct VulkanMetaDescriptorSet
  {
    std::vector<VulkanMetaUniform> m_meta_uniforms;
    inline void push_back(VulkanMetaUniform& uniform_data) { m_meta_uniforms.push_back(uniform_data); }
  };

  struct VulkanUniformMetaData : public EspUniformMetaData
  {
   private:
    inline void push_back_to_current(VulkanMetaUniform& uniform_data)
    {
      m_meta_descriptor_sets.back().push_back(uniform_data);
    }

   public:
    uint32_t m_buffer_uniform_counter  = 0;
    uint32_t m_texture_uniform_counter = 0;

    uint32_t m_binding_count;
    std::vector<VulkanMetaDescriptorSet> m_meta_descriptor_sets;

   public:
    VulkanUniformMetaData();
    virtual ~VulkanUniformMetaData();

    virtual EspUniformMetaData& establish_descriptor_set() override;
    virtual void add_buffer_uniform(EspUniformShaderStage stage,
                                    uint32_t size_of_data_chunk,
                                    uint32_t count_of_data_chunks = 1) override;

    virtual void add_texture_uniform(EspUniformShaderStage stage, uint32_t count_of_texturs = 1) override;

    operator bool() const;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_UNIFORM_HH */