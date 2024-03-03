#include "EspWorkerBuilder.hh"

#include "Platform/Vulkan/Worker/VulkanWorkerBuilder.hh"

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
      m_size{ size }, m_binding{ binding }, m_input_rate{ input_rate }, m_attrs{ std::move(attr) }
  {
  }

  void EspWorkerBuilder::set_polygon_mode(esp::EspPolygonMode mode) { m_polygon_mode = mode; }

  std::unique_ptr<EspWorkerBuilder> EspWorkerBuilder::create()
  {
    /* ---------------------------------------------------------*/
    /* ------------- PLATFORM DEPENDENT ------------------------*/
    /* ---------------------------------------------------------*/
#if ESP_USE_VULKAN
    auto worker_builder = std::make_unique<VulkanWorkerBuilder>();
#else
#error Unfortunatelly, only Vulkan is supported by Espert. Please, install Vulkan API.
#endif
    /* ---------------------------------------------------------*/

    worker_builder->m_polygon_mode = EspPolygonMode::ESP_POLYGON_MODE_FILL;

    return worker_builder;
  }
} // namespace esp
