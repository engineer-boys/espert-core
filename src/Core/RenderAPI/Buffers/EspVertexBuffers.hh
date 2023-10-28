#ifndef RENDER_API_ESP_VERTEX_BUFFER_HH
#define RENDER_API_ESP_VERTEX_BUFFER_HH

#include "EspBuffer.hh"

namespace esp
{
  class EspVertexBuffers
  {
   public:
    class EspVertexBuffer : public EspBuffer
    {
     public:
      static std::unique_ptr<EspVertexBuffer> create(void* data, uint32_t vertex_size, uint32_t vertex_count);

      EspVertexBuffer(const EspVertexBuffer&)            = delete;
      EspVertexBuffer& operator=(const EspVertexBuffer&) = delete;

      EspVertexBuffer()          = default;
      virtual ~EspVertexBuffer() = default;

      virtual void attach() = 0;
    };

   private:
    std::vector<std::unique_ptr<EspVertexBuffer>> m_vertex_buffers{};

   public:
    static std::unique_ptr<EspVertexBuffers> create();

    EspVertexBuffers(const EspVertexBuffers&)            = delete;
    EspVertexBuffers& operator=(const EspVertexBuffers&) = delete;

    ~EspVertexBuffers() = default;

    void add(void* data, uint32_t vertex_size, uint32_t vertex_count);

    void attach();

   private:
    EspVertexBuffers() = default;
  };
} // namespace esp

#endif // RENDER_API_ESP_VERTEX_BUFFER_HH
