#include "EspPipelineBuilder.hh"

#include "Platform/Vulkan/Pipeline/VulkanPipelineBuilder.hh"

namespace esp
{
  EspVertexAttribute::EspVertexAttribute(uint32_t location, EspAttrFormat format, uint32_t offset) :
      m_location{ location }, m_format{ format }, m_offset{ offset }
  {
  }

  EspVertexLayout::EspVertexLayout(uint32_t size,
                                   uint32_t binding,
                                   EspVertexInputRate input_rate,
                                   std::vector<EspVertexAttribute> attr) :
      m_size{ size },
      m_binding{ binding }, m_input_rate{ input_rate }, m_attrs{ std::move(attr) }
  {
  }

  std::unique_ptr<EspWorkerBuilder> EspWorkerBuilder::create()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
    // #if defined(OPENGL_PLATFORM)
    // #elif defined(VULKAN_PLATFORM)
    return std::make_unique<VulkanWorkerBuilder>();
    // #else
    // #error Unfortunatelly, neither Vulkan nor OpenGL is supported.
    // #endif
    /* ---------------------------------------------------------*/
  }
} // namespace esp
