#include "VulkanUniformMetaData.hh"

namespace esp
{
  VulkanMetaUniform::VulkanMetaUniform(EspUniformShaderStage stage,
                                       uint32_t size_of_data_chunk,
                                       uint32_t count_of_elements,
                                       uint32_t binding,
                                       EspUniformType uniform_type) :
      m_stage{ stage },
      m_size_of_data_chunk{ size_of_data_chunk }, m_number_of_elements{ count_of_elements }, m_binding{ binding },
      m_uniform_type{ uniform_type }
  {
  }

  VulkanUniformMetaData::VulkanUniformMetaData() {}
  VulkanUniformMetaData::~VulkanUniformMetaData() {}

  EspUniformMetaData& VulkanUniformMetaData::establish_descriptor_set()
  {
    m_binding_count = 0;
    m_current_ds_counter += 1;
    m_meta_descriptor_sets.push_back(VulkanMetaDescriptorSet());

    return *this;
  }

  void VulkanUniformMetaData::add_buffer_uniform(EspUniformShaderStage stage,
                                                 uint32_t size_of_data_chunk,
                                                 uint32_t count_of_data_chunks)
  {
    ESP_ASSERT(m_current_ds_counter != -1, "You forgot to create descriptor set!!!");

    VulkanMetaUniform uniform(stage,
                              size_of_data_chunk,
                              count_of_data_chunks,
                              m_binding_count,
                              EspUniformType::ESP_BUFFER_UNIFORM);
    push_back_to_current(uniform);

    m_binding_count += 1;
    m_buffer_uniform_counter++;
  }

  void VulkanUniformMetaData::add_texture_uniform(EspUniformShaderStage stage, uint32_t count_of_texturs)
  {
    ESP_ASSERT(m_current_ds_counter != -1, "You forgot to create descriptor set!!!");

    VulkanMetaUniform uniform(stage, 0, count_of_texturs, m_binding_count, EspUniformType::ESP_TEXTURE);
    push_back_to_current(uniform);

    m_binding_count += 1;
    m_texture_uniform_counter++;
  }

  VulkanUniformMetaData::operator bool() const
  {
    return m_buffer_uniform_counter != 0 || m_texture_uniform_counter != 0;
  }
} // namespace esp
