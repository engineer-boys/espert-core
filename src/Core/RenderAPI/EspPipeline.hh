#ifndef CORE_RENDER_API_ESP_PIPELINE_HH
#define CORE_RENDER_API_ESP_PIPELINE_HH

#include "esppch.hh"

namespace esp
{
  class EspPipeline
  {
   public:
    virtual ~EspPipeline() {}

    virtual void attach() const          = 0;
    virtual void attach_uniforms() const = 0;

    virtual void update_buffer_uniform(uint32_t set, uint32_t binding, uint32_t size, void* data)                = 0;
    virtual void update_buffer_uniform(uint32_t set, uint32_t binding, uint32_t elem, uint32_t size, void* data) = 0;
  };
} // namespace esp

#endif /* CORE_RENDER_API_ESP_PIPELINE_HH */
