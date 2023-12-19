#ifndef RENDERER_ESP_DEVICE_HH
#define RENDERER_ESP_DEVICE_HH

#include "esppch.hh"

// Render API Vulkan
#include "VulkanContextData.hh"

// std
#include <string>
#include <vector>

namespace esp
{
  class VulkanDevice
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    static VulkanDevice* s_instance;

    VkPhysicalDevice m_physical_device;
    VkDevice m_device;
    VkPhysicalDeviceProperties m_properties;

    std::vector<const char*> m_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                     VK_KHR_MAINTENANCE1_EXTENSION_NAME,
                                                     VK_KHR_MAINTENANCE2_EXTENSION_NAME,
                                                     VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                                                     VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
                                                     VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
                                                     VK_KHR_MULTIVIEW_EXTENSION_NAME };

    /* -------------------------- METHODS ---------------------------------- */
   private:
    void pick_physical_device(VulkanContextData* context_data);
    void create_logical_device(VulkanContextData* context_data);
    bool is_device_suitable(VkPhysicalDevice device, VulkanContextData* context_data);
    QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VulkanContextData* context_data);
    bool check_device_extension_support(VkPhysicalDevice device);

   public:
    VulkanDevice();
    ~VulkanDevice();

    void init(VulkanContextData* context_data);
    void terminate();

    VulkanDevice(const VulkanDevice&)            = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    VkSampleCountFlagBits get_max_usable_sample_count();

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<VulkanDevice> create(VulkanContextData* context_data);

    static inline const VkPhysicalDevice& get_physical_device() { return s_instance->m_physical_device; }
    static inline const VkDevice& get_logical_device() { return s_instance->m_device; }
    static inline const VkPhysicalDeviceProperties& get_properties() { return s_instance->m_properties; }
    static VkFormatProperties get_format_properties(VkFormat format);

    // -------------------------------------- Swap Chain Helper Functions --------------------------------------
    static VkFormat find_supported_format(const std::vector<VkFormat>& candidates,
                                          VkImageTiling tiling,
                                          VkFormatFeatureFlags features);
    static uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);
    // ---------------------------------------------------------------------------------------------------------
  };

} // namespace esp

#endif // RENDERER_ESP_DEVICE_HH
