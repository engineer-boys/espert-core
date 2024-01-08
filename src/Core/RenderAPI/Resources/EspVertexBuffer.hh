#ifndef RENDER_API_ESP_VERTEX_BUFFER_HH
#define RENDER_API_ESP_VERTEX_BUFFER_HH

#include "EspBuffer.hh"

#include "Core/RenderAPI/PipelineOrdering/EspCommandBuffer.hh"

namespace esp
{
  class EspVertexBuffer : public EspBuffer
  {
   protected:
    uint32_t m_vertex_count;

   public:
    static std::unique_ptr<EspVertexBuffer> create(void* data,
                                                   uint32_t vertex_size,
                                                   uint32_t vertex_count,
                                                   BufferType type = LOCAL);

    EspVertexBuffer(const EspVertexBuffer&)            = delete;
    EspVertexBuffer& operator=(const EspVertexBuffer&) = delete;

    EspVertexBuffer()          = default;
    virtual ~EspVertexBuffer() = default;

    virtual void update(void* data, uint32_t instance_size, uint32_t instance_count, uint64_t offset) = 0;

    inline uint32_t get_vertex_count() { return m_vertex_count; }

    virtual void attach()                                           = 0;
    virtual void attach_instanced(EspVertexBuffer& instance_buffer) = 0;

    virtual void attach(EspCommandBufferId* id)                                             = 0;
    virtual void attach_instanced(EspCommandBufferId* id, EspVertexBuffer& instance_buffer) = 0;
  };
} // namespace esp

#endif // RENDER_API_ESP_VERTEX_BUFFER_HH
