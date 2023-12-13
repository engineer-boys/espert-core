#ifndef ESP_CORE_RENDERAPI_RESOURCES_SPIRVREFLECTION_HH
#define ESP_CORE_RENDERAPI_RESOURCES_SPIRVREFLECTION_HH

#include <esppch.hh>

#include "Core/RenderAPI/Resources/ShaderProperty.hh"

namespace esp
{
  class SpirvReflection
  {
   public:
    static void parse_shader_properties(const spirv_cross::Compiler& compiler,
                                        VkShaderStageFlagBits stage,
                                        std::vector<ShaderProperty>& properties);

    static void parse_push_constants(const spirv_cross::Compiler& compiler,
                                     VkShaderStageFlagBits stage,
                                     std::vector<ShaderProperty>& properties);
  };

} // namespace esp

#endif // ESP_CORE_RENDERAPI_RESOURCES_SPIRVREFLECTION_HH