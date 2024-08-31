#ifndef RENDER_API_ESP_SHADER_HH
#define RENDER_API_ESP_SHADER_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspShaderStage.hh"
#include "Core/RenderAPI/Worker/EspWorkerBuilder.hh"

namespace esp
{
  /// @brief Represents GPU's shader. (encapsulates graphics pipeline)
  class EspShader
  {
   protected:
    EspShader(const std::string& name);

    std::unique_ptr<EspWorkerBuilder> m_worker_builder;
    std::unique_ptr<EspWorker> m_worker;
    std::string m_name;

   public:
    /// @brief Virtual destructor
    virtual ~EspShader() = default;
    PREVENT_COPY(EspShader);

    /// @brief Creates Espshader with the spirv resource.
    /// @param name Name of the shader.
    /// @param spirv_resource SpirvResource containing the shader code.
    /// @return Shared pointer to uninitialized EspShader.
    static std::shared_ptr<EspShader> create(const std::string& name, std::unique_ptr<SpirvResource> spirv_resource);
    /// @brief Attach shader to use it in next operations.
    void attach();
    /// @brief Creates an EspUniformManager that is bound to this shader.
    /// @param start_managed_ds Id of the first descriptor set that this uniform manager will manage.
    /// @param end_managed_ds Id of the last descriptor set that this uniform manager will manage.
    /// @return Newly created EspUniformManager.
    std::unique_ptr<EspUniformManager> create_uniform_manager(int start_managed_ds = -1, int end_managed_ds = -1) const;
    /// @brief Enables depth test in an unconfigured shader.
    /// @param format Type of data to use for depth block.
    /// @param compare_op Operator that will be used in comparisons in depth test.
    void enable_depth_test(EspDepthBlockFormat format, EspCompareOp compare_op);
    /// @brief Enables multisampling.
    /// @param sample_count_flag Sets number of samples.
    void enable_multisampling(EspSampleCountFlag sample_count_flag);
    /// @brief Enables alpha blending.
    void enable_alpha_blending();
    /// @brief Sets pipeline attachement formats.
    /// @param formats List of block formats.
    void set_attachment_formats(std::vector<EspBlockFormat> formats);
    /// @brief Sets the layout of vertices that will be shader's input.
    /// @param vertex_layouts List of structures describing each input.
    void set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts);
    /// @brief Sets specialization constant values for specific shader stages.
    /// @param spec_const_map Map containing specialzation constant data for shader stages.
    void set_specialization(const SpecializationConstantMap& spec_const_map);
    /// @brief Sets uniform metadata that contains information about uniform buffers, texture uniforms and push
    /// constants.
    /// @param uniforms_meta_data Object with data needed to configure uniforms.
    void set_worker_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data);

    void set_rasterizer_settings(EspRasterizerSettings settings);
    void set_input_assembly_settings(EspInputAssemblySettings settings);

    void only_attach() const;
    void set_viewport(EspViewport viewport);
    void set_scissors(EspScissorRect scissor_rect);

    void only_attach(EspCommandBufferId* id) const;
    void set_viewport(EspCommandBufferId* id, EspViewport viewport);
    void set_scissors(EspCommandBufferId* id, EspScissorRect scissor_rect);

    /// @brief
    void build_worker();
  };
} // namespace esp

#endif // RENDER_API_ESP_SHADER_HH
