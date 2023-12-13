#ifndef CORE_RENDER_API_ESP_UNIFORM_META_DATA_HH
#define CORE_RENDER_API_ESP_UNIFORM_META_DATA_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/ShaderProperty.hh"

namespace esp
{
  struct EspUniformMetaData
  {
   protected:
    std::unordered_map<ShaderStage, std::vector<ShaderProperty>> m_shader_properties;

   public:
    virtual ~EspUniformMetaData() {}

    virtual EspUniformMetaData& add_buffer_uniform(ShaderStage stage,
                                                   uint32_t size_of_data_chunk,
                                                   uint32_t count_of_data_chunks = 1) = 0;

    virtual EspUniformMetaData& add_shader_stage_property(ShaderStage stage, ShaderProperty shader_property) = 0;

    virtual EspUniformMetaData& load_shader_stage_data(ShaderStage stage, std::vector<uint32_t> source) = 0;

    virtual EspUniformMetaData& establish_descriptor_set() = 0;

    static std::unique_ptr<EspUniformMetaData> create();
  };

} // namespace esp

#endif /* CORE_RENDER_API_ESP_UNIFORM_META_DATA_HH */
