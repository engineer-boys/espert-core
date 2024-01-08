#ifndef VULKAN_RENDER_API_VULKAN_INDEX_BUFFER_HH
#define VULKAN_RENDER_API_VULKAN_INDEX_BUFFER_HH

#include "Core/RenderAPI/Resources/EspIndexBuffer.hh"
#include "VulkanBuffer.hh"

namespace esp
{
  class VulkanIndexBuffer : public EspIndexBuffer
  {
   private:
    std::unique_ptr<VulkanBuffer> m_index_buffer{};

   public:
    static std::unique_ptr<VulkanIndexBuffer> create(uint32_t* indices, uint32_t index_count);

    VulkanIndexBuffer(const VulkanIndexBuffer&)            = delete;
    VulkanIndexBuffer& operator=(const VulkanIndexBuffer&) = delete;

    ~VulkanIndexBuffer() override = default;

    virtual void attach() override;
    virtual void attach(EspCommandBufferId* id) override;

   private:
    VulkanIndexBuffer() = default;
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_INDEX_BUFFER_HH
