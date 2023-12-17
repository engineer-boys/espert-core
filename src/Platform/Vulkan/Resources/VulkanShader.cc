#include "Platform/Vulkan/Resources/VulkanShader.hh"
#include "Core/RenderAPI/Resources/EspShader.hh"

namespace esp
{
  VulkanShader::VulkanShader(const std::string& name, std::unique_ptr<SpirvResource> spirv_resource) : EspShader(name)
  {
    m_pipeline_builder->set_shaders(std::move(spirv_resource));
  }

  std::shared_ptr<VulkanShader> VulkanShader::create(const std::string name,
                                                     std::unique_ptr<SpirvResource> spirv_resource)
  {
    auto vulkan_shader = std::shared_ptr<VulkanShader>(new VulkanShader(name, std::move(spirv_resource)));

    return vulkan_shader;
  }

  VulkanShader::~VulkanShader() {}
} // namespace esp