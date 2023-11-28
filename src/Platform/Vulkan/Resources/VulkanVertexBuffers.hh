#ifndef VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH
#define VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH

#include "Core/RenderAPI/Resources/EspVertexBuffers.hh"
#include "VulkanBuffer.hh"

namespace esp
{
  class VulkanVertexBuffers : public EspVertexBuffers
  {
   private:
    std::vector<std::unique_ptr<VulkanBuffer>> m_vertex_buffers{};
    std::vector<VkBuffer> m_buffers{};
    std::vector<VkDeviceSize> m_offsets{};

   public:
    static std::unique_ptr<VulkanVertexBuffers> create();

    VulkanVertexBuffers(const VulkanVertexBuffers&)            = delete;
    VulkanVertexBuffers& operator=(const VulkanVertexBuffers&) = delete;

    ~VulkanVertexBuffers() override = default;

    void add(void* data, uint32_t vertex_size, uint32_t vertex_count, BufferType type = LOCAL) override;
    void update(uint32_t index, void* data, uint32_t instance_size, uint32_t instance_count, uint64_t offset) override;
    void attach() override;

    inline uint32_t get_buffers_count() override { return m_vertex_buffers.size(); }

   private:
    VulkanVertexBuffers() = default;
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH
