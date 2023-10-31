#include "EspPipelineBuilder.hh"

#include "Platform/Vulkan/VulkanPipelineBuilder.hh"

namespace esp
{
  EspVertexAttribute::EspVertexAttribute(uint32_t location, EspAttrFormat format, uint32_t offset) :
      m_location(location), m_format(format), m_offset(offset)
  {
  }

  EspVertexLayout::EspVertexLayout(uint32_t size, uint32_t binding, std::vector<EspVertexAttribute> attr) :
      m_size(size), m_binding(binding), m_attrs(attr)
  {
  }

  std::unique_ptr<EspPipelineBuilder> EspPipelineBuilder::create()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    // #elif defined(VULKAN_PLATFORM)
    return std::make_unique<VulaknPipelineBuilder>();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
  }
} // namespace esp
