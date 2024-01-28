#ifndef CORE_RENDER_API_ESP_PIPELINE_HH
#define CORE_RENDER_API_ESP_PIPELINE_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/RenderPlans/EspCommandBuffer.hh"
#include "Core/RenderAPI/Uniforms/EspUniformManager.hh"

namespace esp
{
  struct EspViewport
  {
    float m_width;
    float m_height;
    float m_min_depth;
    float m_max_depth;
  };

  struct EspScissorRect
  {
    uint32_t m_width;
    uint32_t m_height;
    int32_t m_offset_x;
    int32_t m_offset_y;
  };

  class EspWorker
  {
    /* -------------------------- METHODS ---------------------------------- */
   public:
    virtual ~EspWorker() {}

    virtual void attach() const = 0;

    virtual void only_attach() const                       = 0;
    virtual void set_viewport(EspViewport viewport)        = 0;
    virtual void set_scissors(EspScissorRect scissor_rect) = 0;

    virtual void only_attach(EspCommandBufferId* id) const                         = 0;
    virtual void set_viewport(EspCommandBufferId* id, EspViewport viewport)        = 0;
    virtual void set_scissors(EspCommandBufferId* id, EspScissorRect scissor_rect) = 0;

    virtual std::unique_ptr<EspUniformManager> create_uniform_manager(int start_managed_ds = -1,
                                                                      int end_managed_ds   = -1) const = 0;
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_PIPELINE_HH */
