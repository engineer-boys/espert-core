#ifndef VULKAN_RENDER_API_VULKAN_CONTEXT_HH
#define VULKAN_RENDER_API_VULKAN_CONTEXT_HH

#include "Core/RenderAPI/EspRenderContext.hh"

#include "VulkanCommandHandler.hh"
#include "VulkanDevice.hh"

#include "esppch.hh"

// libs
#include "volk.h"

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

  class VulkanContext : public EspRenderContext
  {
   private:
    struct DeviceContextData
    {
      VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
      VkDevice m_device;
    };

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

      const std::vector<const char*> m_validation_layers = { "VK_LAYER_KHRONOS_validation" };

      std::vector<const char*> m_instance_extensions = {};
      std::vector<const char*> m_device_extensions   = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      };

      const bool m_enable_validation_layers = true;
    };

   private:
    static VulkanContext* s_instance;

    DeviceContextData m_device_context_data;
    ContextData m_context_data;

    std::unique_ptr<VulkanCommandHandler> m_vulkan_command_handler{};
    std::unique_ptr<VulkanDevice> m_vulkan_device{};

   private:
    void create_instance();
    void setup_debug_messenger();
    void create_surface(EspWindow& window);
    void pick_physical_device();
    void create_logical_device();

    VulkanContext();

   public:
    VulkanContext(const VulkanContext& other)            = delete;
    VulkanContext& operator=(const VulkanContext& other) = delete;

    /* ---------------- API ------------------------------------------- */
    virtual ~VulkanContext() override = default;

    void init(EspWindow& window) override;
    void terminate() override;
    void update() override;

    static std::unique_ptr<VulkanContext> create();
    static const ContextData& get_context_data();
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_CONTEXT_HH
