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

    std::unique_ptr<EspWorkerBuilder> m_worker_builder;
    std::unique_ptr<EspWorker> m_worker;
    std::string m_name;

   public:
    virtual ~EspShader() = default;
    PREVENT_COPY(EspShader);

    static std::shared_ptr<EspShader> create(const std::string& name, std::unique_ptr<SpirvResource> spirv_resource);
    void attach();
    std::unique_ptr<EspUniformManager> create_uniform_manager(int start_managed_ds = -1, int end_managed_ds = -1) const;
    void enable_depth_test(EspDepthBlockFormat format, EspCompareOp compare_op);
    void enable_multisampling(EspSampleCountFlag sample_count_flag);
    void set_attachment_formats(std::vector<EspBlockFormat> formats);
    void set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts);
    void set_worker_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data);

    void only_attach(EspCommandBufferId* id) const;
    void set_viewport(EspCommandBufferId* id, EspViewport viewport);
    void set_scissors(EspCommandBufferId* id, EspScissorRect scissor_rect);

    void build_worker();
  };
} // namespace esp

#endif // RENDER_API_ESP_SHADER_HH