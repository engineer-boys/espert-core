#ifndef VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH
#define VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH

#include "Core/RenderAPI/Buffers/EspVertexBuffers.hh"
#include "VulkanBuffer.hh"

namespace esp
{
  class VulkanVertexBuffer : public EspVertexBuffers::EspVertexBuffer
  {
    std::unique_ptr<VulkanBuffer> m_vertex_buffer{};

   public:
    static std::unique_ptr<VulkanVertexBuffer> create(void* data, uint32_t vertex_size, uint32_t vertex_count);

    VulkanVertexBuffer(const VulkanVertexBuffer&)            = delete;
    VulkanVertexBuffer& operator=(const VulkanVertexBuffer&) = delete;

    ~VulkanVertexBuffer() override = default;

    virtual void attach() override;

   private:
    VulkanVertexBuffer() = default;
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH
