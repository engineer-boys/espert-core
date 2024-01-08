#ifndef RENDER_API_ESP_INDEX_BUFFER_HH
#define RENDER_API_ESP_INDEX_BUFFER_HH

#include "EspBuffer.hh"

#include "Core/RenderAPI/PipelineOrdering/EspCommandBuffer.hh"

namespace esp
{
  class EspIndexBuffer : public EspBuffer
  {
   protected:
    uint32_t m_index_count;

   public:
    static std::unique_ptr<EspIndexBuffer> create(uint32_t* indices, uint32_t index_count);

    EspIndexBuffer(const EspIndexBuffer&)            = delete;
    EspIndexBuffer& operator=(const EspIndexBuffer&) = delete;

    EspIndexBuffer()          = default;
    virtual ~EspIndexBuffer() = default;

    inline uint32_t get_index_count() { return m_index_count; }

    virtual void attach()                       = 0;
    virtual void attach(EspCommandBufferId* id) = 0;
  };
} // namespace esp

#endif // RENDER_API_ESP_INDEX_BUFFER_HH
