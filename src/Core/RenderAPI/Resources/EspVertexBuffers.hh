#ifndef RENDER_API_ESP_VERTEX_BUFFER_HH
#define RENDER_API_ESP_VERTEX_BUFFER_HH

#include "EspBuffer.hh"

namespace esp
{
  class EspVertexBuffers : public EspBuffer
  {
   public:
    static std::unique_ptr<EspVertexBuffers> create();

    EspVertexBuffers(const EspVertexBuffers&)            = delete;
    EspVertexBuffers& operator=(const EspVertexBuffers&) = delete;

    EspVertexBuffers()          = default;
    virtual ~EspVertexBuffers() = default;

    virtual void add(void* data, uint32_t vertex_size, uint32_t vertex_count, BufferType type = LOCAL) = 0;
    virtual void update(uint32_t index,
                        void* data,
                        uint32_t instance_size,
                        uint32_t instance_count,
                        uint64_t offset)                                                               = 0;
    virtual void attach()                                                                              = 0;

    virtual uint32_t get_buffers_count() = 0;

   private:
  };
} // namespace esp

#endif // RENDER_API_ESP_VERTEX_BUFFER_HH
