#ifndef VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH
#define VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH

#include "Core/RenderAPI/Resources/EspVertexBuffer.hh"
#include "VulkanBuffer.hh"

namespace esp
{
  class VulkanVertexBuffer : public EspVertexBuffer
  {
   private:
    std::unique_ptr<VulkanBuffer> m_vertex_buffer{};

   public:
    static std::unique_ptr<VulkanVertexBuffer> create(void* data,
                                                      uint32_t vertex_size,
                                                      uint32_t vertex_count,
                                                      BufferType type);

    VulkanVertexBuffer(const VulkanVertexBuffer&)            = delete;
    VulkanVertexBuffer& operator=(const VulkanVertexBuffer&) = delete;

    ~VulkanVertexBuffer() override = default;

    void update(void* data, uint32_t instance_size, uint32_t instance_count, uint64_t offset) override;
    void attach() override;
    void attach_instanced(EspVertexBuffer& instance_buffer) override;

   private:
    VulkanVertexBuffer() = default;
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH
