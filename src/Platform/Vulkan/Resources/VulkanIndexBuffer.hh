#ifndef VULKAN_RENDER_API_VULKAN_INDEX_BUFFER_HH
#define VULKAN_RENDER_API_VULKAN_INDEX_BUFFER_HH

#include "Core/RenderAPI/Resources/EspIndexBuffer.hh"
#include "VulkanBuffer.hh"

namespace esp
{
  /// @brief Vulkan's index buffer.
  class VulkanIndexBuffer : public EspIndexBuffer
  {
   private:
    std::unique_ptr<VulkanBuffer> m_index_buffer{};

   public:
    /// @brief Creates Vulkan's index buffer.
    /// @param indices Array of indices.
    /// @param index_count Count of indices in array.
    /// @return Unique pointer to instance of index buffer.
    static std::unique_ptr<VulkanIndexBuffer> create(uint32_t* indices, uint32_t index_count);

    VulkanIndexBuffer(const VulkanIndexBuffer&)            = delete;
    VulkanIndexBuffer& operator=(const VulkanIndexBuffer&) = delete;

    /// @brief Virtual destructor.
    ~VulkanIndexBuffer() override = default;

    /// @brief Attaches index buffer to be used in next operations.
    virtual void attach() override;
    /// @brief Attaches index buffer for specific command buffer.
    /// @param id Id of command buffer to attach index buffer to.
    virtual void attach(EspCommandBufferId* id) override;

   private:
    VulkanIndexBuffer() = default;
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_INDEX_BUFFER_HH
