#ifndef VULKAN_RENDER_API_VULKAN_SHADER_HH
#define VULKAN_RENDER_API_VULKAN_SHADER_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Resources/EspShader.hh"

// Core
#include "Core/Resources/Systems/ShaderSystem.hh"

namespace esp
{
  /// @brief Represents Vulkan's shader. (encapsulates graphics pipeline)
  class VulkanShader : public EspShader
  {
   public:
    /// @brief Creates VulkanShader with the spirv resource.
    /// @param name Name of the shader.
    /// @param spirv_resource SpirvResource containing the shader code.
    /// @return Shared pointer to uninitialized VulkanShader.
    static std::shared_ptr<VulkanShader> create(const std::string name, std::unique_ptr<SpirvResource> spirv_resource);

    PREVENT_COPY(VulkanShader);

    /// @brief Destructor unload's Vulkan's GPU objects.
    ~VulkanShader();

   private:
    VulkanShader(const std::string& name, std::unique_ptr<SpirvResource> spirv_resource);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_SHADER_HH
