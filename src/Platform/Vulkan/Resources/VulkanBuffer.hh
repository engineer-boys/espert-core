#ifndef VULKAN_RENDER_API_VULKAN_BUFFER_HH
#define VULKAN_RENDER_API_VULKAN_BUFFER_HH

#include "esppch.hh"

namespace esp
{
  /// @brief Vulkan's general buffer.
  class VulkanBuffer
  {
   private:
    void* m_mapped          = nullptr;
    VkBuffer m_buffer       = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;

    VkDeviceSize m_buffer_size;
    uint32_t m_instance_count;
    VkDeviceSize m_instance_size;
    VkDeviceSize m_alignment_size;
    VkBufferUsageFlags m_usage_flags;
    VkMemoryPropertyFlags m_memory_property_flags;

   public:
    /// @brief Creates VulkanBuffer and fills it with data.
    /// @param data Raw pointer to some data.
    /// @param instance_size Size of an data instance.
    /// @param instance_count Number of data instances.
    /// @param usage_flags Usage flags tell how the buffer will be used.
    /// @param memory_property_flags Memory properties.
    /// @param min_offset_alignment How the data should be aligned.
    /// @return Unique pointer to instance of created VulkanBuffer.
    static std::unique_ptr<VulkanBuffer> create_and_fill(void* data,
                                                         uint32_t instance_size,
                                                         uint32_t instance_count,
                                                         VkBufferUsageFlags usage_flags,
                                                         VkMemoryPropertyFlags memory_property_flags,
                                                         VkDeviceSize min_offset_alignment = 1);

    /// @brief Constructor that sets informative members.
    /// @param instance_size Size of an data instance.
    /// @param instance_count Number of data instances.
    /// @param usage_flags Usage flags tell how the buffer will be used.
    /// @param memory_property_flags Memory properties.
    /// @param min_offset_alignment How the data should be aligned.
    VulkanBuffer(VkDeviceSize instance_size,
                 uint32_t instance_count,
                 VkBufferUsageFlags usage_flags,
                 VkMemoryPropertyFlags memory_property_flags,
                 VkDeviceSize min_offset_alignment = 1);
    /// @brief Destructor unloads Vulkan's objects from GPU.
    ~VulkanBuffer();

    VulkanBuffer(const VulkanBuffer&)            = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    /// @brief Maps CPU side buffer to GPU side buffer.
    /// @param size Size of memory to map.
    /// @param offset Offset of memory to map.
    /// @return Result of the operation.
    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    /// @brief Unmaps mapped buffers.
    void unmap();

    /// @brief Reads data from buffer.
    /// @param data Pointer to buffer where data will be written to.
    /// @param size Size of data to read.
    /// @param offset Offset of data to read.
    void read_from_buffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    /// @brief Writes data to buffer.
    /// @param data Pointer to data to be written.
    /// @param size Size of data to write.
    /// @param offset Offset of where to write.
    void write_to_buffer(const void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    /// @brief Flushes the buffer.
    /// @param size Size of data to flush.
    /// @param offset Offset of data to flush.
    /// @return Result of the operation.
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    /// @brief Returns buffer descriptor info.
    /// @param size Size of data.
    /// @param offset Offset in buffer.
    /// @return Buffer descriptor info.
    VkDescriptorBufferInfo descriptor_info(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    /// @brief Invalidates buffer.
    /// @param size Size of data to invalidate.
    /// @param offset Offset of data to invalidate.
    /// @return Operation result.
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    /// @brief Write to specific index in buffer.
    /// @param data Pointer to data to write.
    /// @param index Buffer index to write to.
    void write_to_index(void* data, int index);
    /// @brief FLushes data at specific index.
    /// @param index Buffer index of data to flush.
    /// @return Operation result.
    VkResult flush_index(int index);
    /// @brief Returns buffer descriptor info for specific index.
    /// @param index Buffer index to return descriptor info for.
    /// @return Operation result.
    VkDescriptorBufferInfo descriptor_info_for_index(int index);
    /// @brief Invalidates buffer at specific index.
    /// @param index Buffer index to invalidate at.
    /// @return Operation result.
    VkResult invalidate_index(int index);

    /// @brief Returns the Vulkan's buffer object.
    /// @return Vulkan's buffer object.
    inline VkBuffer get_buffer() const { return m_buffer; }
    /// @brief Returns Vulkan's buffer memory.
    /// @return Vulkan's buffer memory.
    inline VkDeviceMemory get_memory() const { return m_memory; }
    /// @brief Returns Vulkan's mapped memory.
    /// @return Vulkan's mapped memory.
    inline void* get_mapped_memory() const { return m_mapped; }
    /// @brief Returns number of instances.
    /// @return Number of instances.
    inline uint32_t get_instance_count() const { return m_instance_count; }
    /// @brief Returns instance size.
    /// @return Instance size.
    inline VkDeviceSize get_instance_size() const { return m_instance_size; }
    /// @brief Returns size of data alignemnt.
    /// @return Size of data alignemnt.
    inline VkDeviceSize get_alignment_size() const { return m_alignment_size; }
    /// @brief Returns buffer's usage flags.
    /// @return Buffer's usage flags.
    inline VkBufferUsageFlags get_usage_flags() const { return m_usage_flags; }
    /// @brief Returns buffer's memory property flags.
    /// @return Buffer's memory property flags.
    inline VkMemoryPropertyFlags get_memory_property_flags() const { return m_memory_property_flags; }
    /// @brief Returns buffer size.
    /// @return Buffer size.
    inline VkDeviceSize get_buffer_size() const { return m_buffer_size; }

   private:
    static VkDeviceSize get_alignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_BUFFER_HH
