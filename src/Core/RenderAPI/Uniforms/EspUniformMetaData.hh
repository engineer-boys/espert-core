#ifndef CORE_RENDER_API_ESP_UNIFORM_META_DATA_HH
#define CORE_RENDER_API_ESP_UNIFORM_META_DATA_HH

#include "esppch.hh"

namespace esp
{
  enum class EspUniformShaderStage
  {
    ESP_VTX_STAGE,
    ESP_FRAG_STAGE,
    ESP_ALL_STAGES
  };

  enum class EspUniformType
  {
    ESP_BUFFER_UNIFORM,
    ESP_TEXTURE,
  };

  struct EspUniformMetaData
  {
   protected:
    // Current descriptor set counter.
    int32_t m_current_ds_counter = -1;

   public:
    virtual ~EspUniformMetaData() {}

    virtual EspUniformMetaData& establish_descriptor_set() = 0;

    virtual EspUniformMetaData& add_buffer_uniform(EspUniformShaderStage stage,
                                                   uint32_t size_of_data_chunk,
                                                   uint32_t count_of_data_chunks = 1) = 0;

    virtual EspUniformMetaData& add_texture_uniform(EspUniformShaderStage stage, uint32_t count_of_textures = 1) = 0;

    virtual EspUniformMetaData& add_push_uniform(EspUniformShaderStage stage, uint32_t offset, uint32_t size) = 0;

    static std::unique_ptr<EspUniformMetaData> create();
  };

} // namespace esp

#endif /* CORE_RENDER_API_ESP_UNIFORM_META_DATA_HH */
