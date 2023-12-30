#ifndef VULKAN_RENDER_API_VULKAN_SHADER_HH
#define VULKAN_RENDER_API_VULKAN_SHADER_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Pipeline/EspPipelineBuilder.hh"
#include "Core/RenderAPI/Resources/EspShader.hh"

// Core
#include "Core/Resources/Systems/ShaderSystem.hh"

#define CHECK_SPV_REFL(result, stage)                \
  ESP_ASSERT((result) == SPV_REFLECT_RESULT_SUCCESS, \
             ("Could not generate reflection data for shader stage " + esp_shader_stage_to_string(stage)).c_str())

namespace esp
{
  using ShaderModuleMap = std::unordered_map<EspShaderStage, spv_reflect::ShaderModule>;

  class VulkanShader : public EspShader
  {
   public:
    static std::shared_ptr<VulkanShader> create(const std::string name,
                                                std::unique_ptr<SpirvResource> spirv_resource,
                                                const EspShaderConfig& config = {});

    PREVENT_COPY(VulkanShader);

    ~VulkanShader();

   private:
    VulkanShader(const std::string& name, std::unique_ptr<EspWorker> worker);
    static std::unique_ptr<EspUniformMetaData> generate_uniform_metadata(const ShaderModuleMap& shader_module_map);
    static std::vector<EspVertexLayout> generate_vertex_layouts(const ShaderModuleMap& shader_module_map,
                                                                const std::vector<EspShaderVertexInputConfig>& config);
    static ShaderModuleMap generate_shader_modules(const SpirvResource& spirv_resource);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_SHADER_HH
