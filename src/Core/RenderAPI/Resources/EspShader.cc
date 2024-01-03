#include "Core/RenderAPI/Resources/EspShader.hh"
#include "Platform/Vulkan/Resources/VulkanShader.hh"

namespace esp
{
  EspShader::EspShader(const std::string& name, std::unique_ptr<EspWorker> worker) :
      m_name(name), m_worker(std::move(worker))
  {
  }

  std::shared_ptr<EspShader> EspShader::create(const std::string& name,
                                               std::unique_ptr<SpirvResource> spirv_resource,
                                               const EspShaderConfig& config)
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    //     auto context = std::make_unique<OpenGLContext>();
    // #elif defined(VULKAN_PLATFORM)
    auto shader = VulkanShader::create(name, std::move(spirv_resource), config);
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

  void EspShader::attach() { m_worker->attach(); }
} // namespace esp
