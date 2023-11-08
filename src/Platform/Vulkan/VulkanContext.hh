#ifndef VULKAN_RENDER_API_VULKAN_CONTEXT_HH
#define VULKAN_RENDER_API_VULKAN_CONTEXT_HH

#include "Core/RenderAPI/EspRenderContext.hh"

#include "VulkanCommandHandler.hh"
#include "VulkanDevice.hh"
#include "VulkanResourceManager.hh"

#include "esppch.hh"

namespace esp
{
  struct SwapChainSupportDetails
  {
    VkSurfaceCapabilitiesKHR m_capabilities;
    std::vector<VkSurfaceFormatKHR> m_formats;
    std::vector<VkPresentModeKHR> m_present_modes;
  };

  struct QueueFamilyIndices
  {
    uint32_t m_graphics_family;
    uint32_t m_present_family;
    bool m_graphics_family_has_value = false;
    bool m_present_family_has_value  = false;

    bool is_complete() const { return m_graphics_family_has_value && m_present_family_has_value; }
  };

  struct DeviceContextData
  {
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkDevice m_device;
    VkPhysicalDeviceProperties m_properties;
  };

  class VulkanContext : public EspRenderContext
  {
   public:
    struct ContextData
    {
      VkInstance m_instance;
      VkDebugUtilsMessengerEXT m_debug_messenger;

      VkSurfaceKHR m_surface;

      QueueFamilyIndices m_queue_family_indices;
      VkQueue m_graphics_queue;
      VkQueue m_present_queue;

      SwapChainSupportDetails m_swap_chain_support_details;

      VkSampleCountFlagBits m_msaa_samples = VK_SAMPLE_COUNT_1_BIT;

      const std::vector<const char*> m_validation_layers = { "VK_LAYER_KHRONOS_validation" };

      std::vector<const char*> m_instance_extensions = {};
      std::vector<const char*> m_device_extensions   = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_maintenance1" };

#ifdef NDEBUG
      const bool m_enable_validation_layers = false;
#else
      const bool m_enable_validation_layers = true;
#endif
    };

   private:
    static VulkanContext* s_instance;

    ContextData m_context_data;

    std::unique_ptr<VulkanDevice> m_vulkan_device{};
    std::unique_ptr<VulkanResourceManager> m_vulkan_resource_manager{};

   public:
    static std::unique_ptr<VulkanContext> create();

    VulkanContext(const VulkanContext& other)            = delete;
    VulkanContext& operator=(const VulkanContext& other) = delete;

    // ------------------------ API ------------------------
    ~VulkanContext() override = default;

    void init(EspWindow& window) override;
    void terminate() override;
    // -----------------------------------------------------

    static const ContextData& get_context_data();

   private:
    VulkanContext();

    void create_instance();
    void setup_debug_messenger();
    void create_surface(EspWindow& window);
    void create_vulkan_device();
    void create_default_sampler();
    void create_vulkan_resource_manager();
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_CONTEXT_HH
