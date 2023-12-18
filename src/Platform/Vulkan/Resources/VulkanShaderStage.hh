#ifndef VULKAN_RENDER_API_VULKAN_SHADER_STAGE_HH
#define VULKAN_RENDER_API_VULKAN_SHADER_STAGE_HH

#include "esppch.hh"

#include "Core/RenderAPI/Resources/EspShaderStage.hh"

namespace esp
{
  VkShaderStageFlagBits esp_shader_stage_to_vk(EspShaderStage shader_stage)
  {
    switch (shader_stage)
    {
    case EspShaderStage::VERTEX:
      return VK_SHADER_STAGE_VERTEX_BIT;

    case EspShaderStage::TESSELATION_CONTROL:
      return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

    case EspShaderStage::TESSELATION_EVALUATION:
      return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

    case EspShaderStage::GEOMETRY:
      return VK_SHADER_STAGE_GEOMETRY_BIT;

    case EspShaderStage::FRAGMENT:
      return VK_SHADER_STAGE_FRAGMENT_BIT;

    case EspShaderStage::COMPUTE:
      return VK_SHADER_STAGE_COMPUTE_BIT;

    default:
      return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    }
  }
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_SHADER_STAGE_HH
