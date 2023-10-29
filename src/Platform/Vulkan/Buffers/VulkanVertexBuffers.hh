#ifndef VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH
#define VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH

#include "Core/RenderAPI/Buffers/EspVertexBuffers.hh"
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

    void add(void* data, uint32_t vertex_size, uint32_t vertex_count) override;
    void attach() override;

   private:
    VulkanVertexBuffers() = default;
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_VERTEX_BUFFER_HH
