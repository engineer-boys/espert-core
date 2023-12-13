#ifndef PLATFORM_VULKAN_RENDER_API_ESP_UNIFORM_DATA_STORAGE_HH
#define PLATFORM_VULKAN_RENDER_API_ESP_UNIFORM_DATA_STORAGE_HH

#include "esppch.hh"

// platfrom
#include "Platform/Vulkan/Resources/VulkanBuffer.hh"
#include "VulkanUniformMetaData.hh"

// std
#include <map>

namespace esp
{
  struct EspUniformDataStorage
  {
   public:
    std::vector<VkDescriptorSetLayout> m_descriptor_set_layouts;
    std::unique_ptr<VulkanUniformMetaData> m_meta_data;

   public:
    inline const VkDescriptorSetLayout* get_layouts_data() const { return m_descriptor_set_layouts.data(); }
    inline const VkPushConstantRange* get_push_constant_data() const
    {
      return m_meta_data->m_push_constant_ranges.data();
    }
    inline uint32_t get_layouts_number() const { return static_cast<uint32_t>(m_descriptor_set_layouts.size()); }
    inline uint32_t get_push_constants_number() const
    {
      return static_cast<uint32_t>(m_meta_data->m_push_constant_ranges.size());
    }

   public:
    EspUniformDataStorage& operator=(const EspUniformDataStorage& other) = delete;
    EspUniformDataStorage(const EspUniformDataStorage& other)            = delete;

    EspUniformDataStorage(std::unique_ptr<VulkanUniformMetaData> meta_data);
    ~EspUniformDataStorage();
  };

} // namespace esp

#endif // PLATFORM_VULKAN_RENDER_API_ESP_UNIFORM_DATA_STORAGE_HH