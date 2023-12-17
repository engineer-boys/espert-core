#include "Core/RenderAPI/Resources/EspShader.hh"
#include "Platform/Vulkan/Resources/VulkanShader.hh"

namespace esp
{
  EspShader::EspShader(const std::string& name) : m_name(name), m_pipeline_builder(EspPipelineBuilder::create()) {}

  std::shared_ptr<EspShader> EspShader::create(const std::string& name, std::unique_ptr<SpirvResource> spirv_resource)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto shader = VulkanShader::create(name, std::move(spirv_resource));
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/

    return shader;
  }

  std::unique_ptr<EspUniformManager> EspShader::create_uniform_manager() const
  {
    return m_pipeline->create_uniform_manager();
  }

  void EspShader::set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts)
  {
    m_pipeline_builder->set_vertex_layouts(std::move(vertex_layouts));
  }

  void EspShader::set_pipeline_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data)
  {
    m_pipeline_builder->set_pipeline_layout(std::move(uniforms_meta_data));
  }

  void EspShader::build_pipeline() { m_pipeline_builder->build_pipeline(); }
} // namespace esp
