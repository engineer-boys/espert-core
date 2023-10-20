#ifndef RENDERER_ESP_BUFFER_H_
#define RENDERER_ESP_BUFFER_H_

#include "EspDevice.hh"

namespace esp
{
  class EspBuffer
  {
   private:
    EspDevice& m_device;
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
    EspBuffer(EspDevice& device, VkDeviceSize instance_size,
              uint32_t instance_count, VkBufferUsageFlags usage_flags,
              VkMemoryPropertyFlags memory_property_flags,
              VkDeviceSize min_offset_alignment = 1);
    ~EspBuffer();

    EspBuffer(const EspBuffer&)            = delete;
    EspBuffer& operator=(const EspBuffer&) = delete;

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();

    void write_to_buffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE,
                         VkDeviceSize offset = 0);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptor_info(VkDeviceSize size   = VK_WHOLE_SIZE,
                                           VkDeviceSize offset = 0);
    VkResult invalidate(VkDeviceSize size   = VK_WHOLE_SIZE,
                        VkDeviceSize offset = 0);

    void write_to_index(void* data, int index);
    VkResult flush_index(int index);
    VkDescriptorBufferInfo descriptor_info_for_index(int index);
    VkResult invalidate_index(int index);

    inline VkBuffer get_buffer() const { return m_buffer; }
    inline void* get_mapped_memory() const { return m_mapped; }
    inline uint32_t get_instance_count() const { return m_instance_count; }
    inline VkDeviceSize get_instance_size() const { return m_instance_size; }
    inline VkDeviceSize get_alignment_size() const { return m_alignment_size; }
    inline VkBufferUsageFlags get_usage_flags() const { return m_usage_flags; }
    inline VkMemoryPropertyFlags get_memory_property_flags() const
    {
      return m_memory_property_flags;
    }
    inline VkDeviceSize get_buffer_size() const { return m_buffer_size; }

   private:
    static VkDeviceSize get_alignment(VkDeviceSize instance_size,
                                      VkDeviceSize min_offset_alignment);
  };
} // namespace esp

#endif // RENDERER_ESP_BUFFER_H_
