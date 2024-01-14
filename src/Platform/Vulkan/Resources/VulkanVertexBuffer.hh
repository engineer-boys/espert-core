#ifndef VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH
#define VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH

#include "Core/RenderAPI/Resources/EspVertexBuffer.hh"
#include "VulkanBuffer.hh"

namespace esp
{
    /// @brief Vulkan's buffer of vertices that will be input for shaders.
  class VulkanVertexBuffer : public EspVertexBuffer
  {
   private:
    std::unique_ptr<VulkanBuffer> m_vertex_buffer{};

   public:
    /// @brief Creates VulkanVertexBuffer.
    /// @param data Raw void pointer to vertex data.
    /// @param vertex_size Size of vertex data.
    /// @param vertex_count Number of vertices.
    /// @param type Type of buffer.
    /// @return Unique pointer to the created buffer.
    static std::unique_ptr<VulkanVertexBuffer> create(void* data,
                                                      uint32_t vertex_size,
                                                      uint32_t vertex_count,
                                                      BufferType type);

    VulkanVertexBuffer(const VulkanVertexBuffer&)            = delete;
    VulkanVertexBuffer& operator=(const VulkanVertexBuffer&) = delete;

    /// @brief Virtual destructor.
    ~VulkanVertexBuffer() override = default;

    /// @brief Updates buffer data.
    /// @param data Raw pointer to vertex data.
    /// @param instance_size Size of vertex data.
    /// @param instance_count Number of vertices.
    /// @param offset Offset of the vertex data.
    void update(void* data, uint32_t instance_size, uint32_t instance_count, uint64_t offset) override;
    /// @brief Attaches the buffer to be used in next operations.
    void attach() override;
    /// @brief Attaches the buffer as well as an instance buffer to be used in next operations.
    void attach_instanced(EspVertexBuffer& instance_buffer) override;

    /// @brief Attaches the buffer to specific EspCommandBuffer to be used in next operations.
    /// @param id Id of the EspCommandBufferId.
    virtual void attach(EspCommandBufferId* id) override;
    /// @brief Attaches the buffer as well as an instance buffer to specific EspCommandBuffer to be used in next operations.
    /// @param id Id of the EspCommandBufferId.
    virtual void attach_instanced(EspCommandBufferId* id, EspVertexBuffer& instance_buffer) override;

   private:
    VulkanVertexBuffer() = default;
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH
