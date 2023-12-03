#include "VulkanUniformMetaData.hh"

namespace esp
{
  EspMetaPush::EspMetaPush(esp::EspUniformShaderStage stage, uint32_t offset, uint32_t size) :
      m_stage{ stage }, m_offset{ offset }, m_size{ size }
  {
  }

  EspMetaUniform::EspMetaUniform(EspUniformShaderStage stage,
                                 uint32_t size_of_data_chunk,
                                 uint32_t count_of_elements,
                                 uint32_t binding,
                                 EspUniformType uniform_type) :
      m_stage{ stage },
      m_size_of_data_chunk{ size_of_data_chunk }, m_number_of_elements{ count_of_elements }, m_binding{ binding },
      m_uniform_type{ uniform_type }
  {
  }

  EspMetaDescriptorSet::EspMetaDescriptorSet(uint32_t set_index) : m_set_index{ set_index } {}

  VulkanUniformMetaData::VulkanUniformMetaData() {}
  VulkanUniformMetaData::~VulkanUniformMetaData() {}

  EspUniformMetaData& VulkanUniformMetaData::establish_descriptor_set()
  {
    m_binding_count = 0;
    m_current_ds_counter += 1;
    m_meta_descriptor_sets.push_back(EspMetaDescriptorSet(m_current_ds_counter));

    return *this;
  }

  EspUniformMetaData& VulkanUniformMetaData::add_buffer_uniform(EspUniformShaderStage stage,
                                                                uint32_t size_of_data_chunk,
                                                                uint32_t count_of_data_chunks)
  {
    ESP_ASSERT(m_current_ds_counter != -1, "You forgot to create descriptor set!!!");
    push_back_to_current_meta_ds(EspMetaUniform(stage,
                                                size_of_data_chunk,
                                                count_of_data_chunks,
                                                m_binding_count,
                                                EspUniformType::ESP_BUFFER_UNIFORM));

    m_binding_count += 1;
    m_general_buffer_uniform_counter++;
    m_meta_descriptor_sets.back().m_buffer_uniform_counter++;

    return *this;
  }

  EspUniformMetaData& VulkanUniformMetaData::add_texture_uniform(EspUniformShaderStage stage,
                                                                 uint32_t count_of_textures)
  {
    ESP_ASSERT(m_current_ds_counter != -1, "You forgot to create descriptor set!!!");

    push_back_to_current_meta_ds(
        EspMetaUniform(stage, 0, count_of_textures, m_binding_count, EspUniformType::ESP_TEXTURE));

    m_binding_count += 1;
    m_general_texture_uniform_counter++;
    m_meta_descriptor_sets.back().m_texture_uniform_counter++;

    return *this;
  }

  EspUniformMetaData& VulkanUniformMetaData::add_push_uniform(EspUniformShaderStage stage,
                                                              uint32_t offset,
                                                              uint32_t size)
  {
    ESP_ASSERT(offset + size <= EspMetaPush::MAX_PUSH_SIZE, "Offset + size can't be larger than MAX_PUSH_SIZE")
    ESP_ASSERT(!m_occupied_push_memory.any(offset, size), "Some of push memory is already in use")

    m_occupied_push_memory.set(offset, size);

    m_meta_pushes.emplace_back(stage, offset, size);
    m_general_push_uniform_counter++;

    return *this;
  }

  VulkanUniformMetaData::operator bool() const
  {
    return m_general_buffer_uniform_counter != 0 || m_general_texture_uniform_counter != 0 ||
        m_general_push_uniform_counter != 0;
  }
} // namespace esp
