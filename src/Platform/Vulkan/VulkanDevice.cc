#include "VulkanDevice.hh"
#include "VulkanCommandHandler.hh"

namespace esp
{
  VulkanDevice* VulkanDevice::s_instance = nullptr;

  std::unique_ptr<VulkanDevice>
  VulkanDevice::create(VkPhysicalDevice physical_device, VkDevice device, VkPhysicalDeviceProperties properties)
  {
    return std::unique_ptr<VulkanDevice>(new VulkanDevice(physical_device, device, properties));
  }

  VulkanDevice::~VulkanDevice() { s_instance = nullptr; }

  void VulkanDevice::complete_queues() { vkDeviceWaitIdle(s_instance->m_device); }

  void VulkanDevice::terminate() { vkDestroyDevice(m_device, nullptr); }

  VkFormat VulkanDevice::find_supported_format(const std::vector<VkFormat>& candidates,
                                               VkImageTiling tiling,
                                               VkFormatFeatureFlags features)
  {
    for (VkFormat format : candidates)
    {
      VkFormatProperties props;
      vkGetPhysicalDeviceFormatProperties(s_instance->m_physical_device, format, &props);

      if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) { return format; }
      if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) { return format; }
    }

    ESP_CORE_ERROR("Failed to find supported format");
    throw std::runtime_error("Failed to find supported format");
  }

  uint32_t VulkanDevice::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties)
  {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(s_instance->m_physical_device, &mem_properties);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
    {
      if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
      {
        return i;
      }
    }

    ESP_CORE_ERROR("Failed to find suitable memory type");
    throw std::runtime_error("Failed to find suitable memory type");
  }

  VulkanDevice::VulkanDevice(VkPhysicalDevice physical_device, VkDevice device, VkPhysicalDeviceProperties properties)
  {
    ESP_ASSERT(VulkanDevice::s_instance == nullptr, "Vulkan device already exists")

    s_instance = this;

    m_physical_device = physical_device;
    m_device          = device;
    m_properties      = properties;
  }
} // namespace esp