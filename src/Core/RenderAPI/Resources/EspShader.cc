#include "Core/RenderAPI/Resources/EspShader.hh"
#include "Platform/Vulkan/Resources/VulkanShader.hh"

namespace esp
{
  EspShader::EspShader(const std::string& name) : m_name(name), m_worker_builder(EspWorkerBuilder::create()) {}

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

  std::unique_ptr<EspUniformManager> EspShader::create_uniform_manager(int start_managed_ds, int end_managed_ds) const
  {
    return m_worker->create_uniform_manager(start_managed_ds, end_managed_ds);
  }

  void EspShader::enable_depth_test(EspDepthBlockFormat format, EspCompareOp compare_op)
  {
    m_worker_builder->enable_depth_test(format, compare_op);
  }

  void EspShader::enable_multisampling(EspSampleCountFlag sample_count_flag)
  {
    m_worker_builder->enable_multisampling(sample_count_flag);
  }

  void EspShader::set_attachment_formats(std::vector<EspBlockFormat> formats) {}

  void EspShader::attach() { m_worker->attach(); }

  void EspShader::set_vertex_layouts(std::vector<EspVertexLayout> vertex_layouts)
  {
    m_worker_builder->set_vertex_layouts(std::move(vertex_layouts));
  }

  void EspShader::set_worker_layout(std::unique_ptr<EspUniformMetaData> uniforms_meta_data)
  {
    m_worker_builder->set_worker_layout(std::move(uniforms_meta_data));
  }

  void EspShader::build_worker() { m_worker = m_worker_builder->build_worker(); }
} // namespace esp
