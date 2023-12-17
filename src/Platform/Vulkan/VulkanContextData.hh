#ifndef VULKAN_RENDER_API_VULKAN_CONTEXT_DATA_HH
#define VULKAN_RENDER_API_VULKAN_CONTEXT_DATA_HH

#include "esppch.hh"

// std
#include <optional>

namespace esp
{
  struct QueueFamilyIndices
  {
    std::optional<uint32_t> m_graphics_family;
    std::optional<uint32_t> m_present_family;

    bool is_complete() const { return m_graphics_family.has_value() && m_present_family.has_value(); }
  };

  struct VulkanContextData
  {
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;

    VkSurfaceKHR m_surface;

    QueueFamilyIndices m_queue_family_indices;
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;

    // VkSampleCountFlagBits m_msaa_samples = VK_SAMPLE_COUNT_1_BIT;

    const std::vector<const char*> m_validation_layers = { "VK_LAYER_KHRONOS_validation" };

    std::vector<const char*> m_instance_extensions = { VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME };

#ifdef NDEBUG
    const bool m_enable_validation_layers = false;
#else
    const bool m_enable_validation_layers = true;
#endif
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_CONTEXT_DATA_HH