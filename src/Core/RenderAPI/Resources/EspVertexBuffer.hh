#ifndef RENDER_API_ESP_VERTEX_BUFFER_HH
#define RENDER_API_ESP_VERTEX_BUFFER_HH

#include "EspBuffer.hh"

#include "Core/RenderAPI/RenderPlans/EspCommandBuffer.hh"

namespace esp
{
  /// @brief Buffer of vertices that will be input for shaders.
  class EspVertexBuffer : public EspBuffer
  {
   protected:
    uint32_t m_vertex_count;

   public:
    /// @brief Creates EspVertexBuffer.
    /// @param data Raw void pointer to vertex data.
    /// @param vertex_size Size of vertex data.
    /// @param vertex_count Number of vertices.
    /// @param type Type of buffer.
    /// @return Unique pointer to the created buffer.
    static std::unique_ptr<EspVertexBuffer> create(void* data,
                                                   uint32_t vertex_size,
                                                   uint32_t vertex_count,
                                                   BufferType type = LOCAL);

    EspVertexBuffer(const EspVertexBuffer&)            = delete;
    EspVertexBuffer& operator=(const EspVertexBuffer&) = delete;

    /// @brief Default constructor.
    EspVertexBuffer() = default;
    /// @brief Cirtual destructor.
    virtual ~EspVertexBuffer() = default;

    /// @brief Updates buffer data.
    /// @param data Raw pointer to vertex data.
    /// @param instance_size Size of vertex data.
    /// @param instance_count Number of vertices.
    /// @param offset Offset of the vertex data.
    virtual void update(void* data, uint32_t instance_size, uint32_t instance_count, uint64_t offset) = 0;

    /// @brief Returns number of vertices in buffer.
    /// @return Number of vertices in buffer.
    inline uint32_t get_vertex_count() { return m_vertex_count; }

    /// @brief Attaches the buffer to be used in next operations.
    virtual void attach() = 0;
    /// @brief Attaches the buffer as well as an instance buffer to be used in next operations.
    virtual void attach_instanced(EspVertexBuffer& instance_buffer) = 0;

    /// @brief Attaches the buffer to specific EspCommandBuffer to be used in next operations.
    /// @param id Id of the EspCommandBufferId.
    virtual void attach(EspCommandBufferId* id) = 0;
    /// @brief Attaches the buffer as well as an instance buffer to specific EspCommandBuffer to be used in next
    /// operations.
    /// @param id Id of the EspCommandBufferId.
    virtual void attach_instanced(EspCommandBufferId* id, EspVertexBuffer& instance_buffer) = 0;
  };
} // namespace esp

#endif // RENDER_API_ESP_VERTEX_BUFFER_HH
