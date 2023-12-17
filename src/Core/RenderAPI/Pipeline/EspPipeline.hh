#ifndef CORE_RENDER_API_ESP_PIPELINE_HH
#define CORE_RENDER_API_ESP_PIPELINE_HH

#include "esppch.hh"

// Render API
#include "Core/RenderAPI/Uniforms/EspUniformManager.hh"

namespace esp
{
  class EspPipeline
  {
   public:
    virtual ~EspPipeline() {}

    virtual void attach() const = 0;

    virtual std::unique_ptr<EspUniformManager> create_uniform_manager(int start_managed_ds = -1,
                                                                      int end_managed_ds   = -1) const = 0;
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_PIPELINE_HH */
