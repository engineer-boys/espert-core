#ifndef CORE_RENDER_API_ESP_PIPELINE_BUILDER_HH
#define CORE_RENDER_API_ESP_PIPELINE_BUILDER_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Pipeline/Types/EspCompareOp.hh"
#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspDepthBlockFormat.hh"
#include "Core/RenderAPI/PipelineOrdering/Block/Types/EspSampleCountFlag.hh"
#include "Core/RenderAPI/Uniforms/EspUniformMetaData.hh"
#include "EspAttrFormat.hh"
#include "EspPipeline.hh"

// Resources
#include "Core/Resources/ResourceTypes.hh"

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

  class EspWorkerBuilder
  {
    /* -------------------------- METHODS ---------------------------------- */
   public:
    virtual ~EspWorkerBuilder() {}

    virtual void enable_depth_test(EspDepthBlockFormat format, EspCompareOp compare_op) = 0;
    virtual void enable_multisampling(EspSampleCountFlag sample_count_flag)             = 0;

    virtual void set_attachment_formats(std::vector<EspBlockFormat> formats) = 0;

    virtual void set_shaders(std::unique_ptr<SpirvResource> spirv_resource) = 0;

    virtual void set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts)           = 0;
    virtual void set_worker_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data) = 0;

    virtual std::unique_ptr<EspWorker> build_worker() = 0;

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<EspWorkerBuilder> create();
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
