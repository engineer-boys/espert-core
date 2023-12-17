#ifndef VULKAN_RENDER_API_VULKAN_SHADER_HH
#define VULKAN_RENDER_API_VULKAN_SHADER_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Resources/EspShader.hh"

// Core
#include "Core/Systems/ShaderSystem.hh"

namespace esp
{
  class VulkanShader : public EspShader
  {
   public:
    static std::shared_ptr<VulkanShader> create(const std::string name, std::unique_ptr<SpirvResource> spirv_resource);

    PREVENT_COPY(VulkanShader);

    ~VulkanShader();

   private:
    VulkanShader(const std::string& name, std::unique_ptr<SpirvResource> spirv_resource);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_SHADER_HH
