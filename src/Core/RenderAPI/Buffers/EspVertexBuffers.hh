#ifndef RENDER_API_ESP_VERTEX_BUFFER_HH
#define RENDER_API_ESP_VERTEX_BUFFER_HH

#include "EspBuffer.hh"

namespace esp
{
  class EspVertexBuffers : EspBuffer
  {
   public:
    static std::unique_ptr<EspVertexBuffers> create();

    EspVertexBuffers(const EspVertexBuffers&)            = delete;
    EspVertexBuffers& operator=(const EspVertexBuffers&) = delete;

    EspVertexBuffers()          = default;
    virtual ~EspVertexBuffers() = default;

    virtual void add(void* data, uint32_t vertex_size, uint32_t vertex_count) = 0;
    virtual void attach()                                                     = 0;

   private:
  };
} // namespace esp

#endif // RENDER_API_ESP_VERTEX_BUFFER_HH
