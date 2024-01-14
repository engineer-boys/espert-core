#ifndef RENDER_API_ESP_INDEX_BUFFER_HH
#define RENDER_API_ESP_INDEX_BUFFER_HH

#include "EspBuffer.hh"

#include "Core/RenderAPI/PipelineOrdering/EspCommandBuffer.hh"

namespace esp
{
  /// @brief Interface for GPU's index buffer.
  class EspIndexBuffer : public EspBuffer
  {
   protected:
    uint32_t m_index_count;

   public:
    /// @brief Creates index buffer for current graphic's API.
    /// @param indices Array of indices.
    /// @param index_count Count of indices in array.
    /// @return Unique pointer to instance of index buffer.
    static std::unique_ptr<EspIndexBuffer> create(uint32_t* indices, uint32_t index_count);

    EspIndexBuffer(const EspIndexBuffer&)            = delete;
    EspIndexBuffer& operator=(const EspIndexBuffer&) = delete;

    /// @brief Default constructor.
    EspIndexBuffer()          = default;
    /// @brief Virtual destructor.
    virtual ~EspIndexBuffer() = default;

    /// @brief Returns number of indices.
    /// @return Number ofindices.
    inline uint32_t get_index_count() { return m_index_count; }

    /// @brief Attaches index buffer to be used in next operations.
    virtual void attach()                       = 0;

    /// @brief Attaches index buffer for specific command buffer.
    /// @param id Id of command buffer to attach index buffer to.
    virtual void attach(EspCommandBufferId* id) = 0;
  };
} // namespace esp

#endif // RENDER_API_ESP_INDEX_BUFFER_HH
