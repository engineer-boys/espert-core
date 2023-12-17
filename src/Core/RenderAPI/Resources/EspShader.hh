#ifndef RENDER_API_ESP_SHADER_HH
#define RENDER_API_ESP_SHADER_HH

#include "esppch.hh"

#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"
#include "Core/RenderAPI/Resources/EspShaderStage.hh"

namespace esp
{
  class EspShader
  {
   protected:
    EspShader(const std::string& name);

    std::unique_ptr<EspPipeline> m_pipeline;
    std::unique_ptr<EspPipelineBuilder> m_pipeline_builder;
    std::string m_name;

   public:
    virtual ~EspShader() = default;
    PREVENT_COPY(EspShader);

    static std::shared_ptr<EspShader> create(const std::string& name, std::unique_ptr<SpirvResource> spirv_resource);
    std::unique_ptr<EspUniformManager> create_uniform_manager() const;
    void set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts);
    void set_pipeline_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data);
    void build_pipeline();
  };
} // namespace esp

#endif // RENDER_API_ESP_SHADER_HH