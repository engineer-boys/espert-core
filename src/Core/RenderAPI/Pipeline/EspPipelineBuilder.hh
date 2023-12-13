#ifndef CORE_RENDER_API_ESP_PIPELINE_BUILDER_HH
#define CORE_RENDER_API_ESP_PIPELINE_BUILDER_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Uniforms/EspUniformMetaData.hh"
#include "EspAttrFormat.hh"
#include "EspPipeline.hh"

namespace esp
{
  struct EspVertexAttribute
  {
    uint32_t m_location;
    EspAttrFormat m_format;
    uint32_t m_offset;

    EspVertexAttribute(uint32_t location, EspAttrFormat format, uint32_t offset);
  };

  struct EspVertexLayout
  {
    uint32_t m_size;
    uint32_t m_binding;
    EspVertexInputRate m_input_rate;
    std::vector<EspVertexAttribute> m_attrs;

    EspVertexLayout(uint32_t size,
                    uint32_t binding,
                    EspVertexInputRate input_rate,
                    std::vector<EspVertexAttribute> attr);
  };

  class EspPipelineBuilder
  {
   public:
    virtual ~EspPipelineBuilder() {}

    virtual void set_shader(ShaderStage stage, std::vector<uint32_t> shader_source) = 0;

    virtual void set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts)             = 0;
    virtual void set_pipeline_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data) = 0;

    virtual std::unique_ptr<EspPipeline> build_pipeline() = 0;

    static std::unique_ptr<EspPipelineBuilder> create();
  };
} // namespace esp

#define VTX_LAYOUT(size, binding, input_rate, ...)   \
  {                                                  \
    (size), (binding), (input_rate), { __VA_ARGS__ } \
  }

#define ATTR(location, format, offset) \
  {                                    \
    (location), format, (offset)       \
  }

#endif /* CORE_RENDER_API_ESP_PIPELINE_BUILDER_HH */
