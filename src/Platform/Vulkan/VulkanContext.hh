#ifndef VULKAN_RENDER_API_VULKAN_CONTEXT_HH
#define VULKAN_RENDER_API_VULKAN_CONTEXT_HH

#include "Core/RenderAPI/EspRenderContext.hh"
#include "esppch.hh"

// libs
#include "volk.h"

namespace esp
{
  struct QueueFamilyIndices
  {
    uint32_t m_graphics_family;
    uint32_t m_present_family;
    bool m_graphics_family_has_value = false;
    bool m_present_family_has_value  = false;

    bool isComplete() const { return m_graphics_family_has_value && m_present_family_has_value; }
  };

  class VulkanContext : public EspRenderContext
  {
   public:
    struct ContextData
    {
      VkInstance m_instance;
      VkDebugUtilsMessengerEXT m_debug_messenger;

      VkSurfaceKHR m_surface;

      VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
      VkDevice m_device;

      QueueFamilyIndices m_queue_family_indices;
      VkQueue m_graphics_queue;
      VkQueue m_present_queue;

      const std::vector<const char*> m_validation_layers = { "VK_LAYER_KHRONOS_validation" };

      std::vector<const char*> m_instance_extensions = {};
      std::vector<const char*> m_device_extensions   = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      };

      const bool m_enable_validation_layers = true;
    };

   private:
    static VulkanContext* s_instance;
    VulkanContext();

   private:
    ContextData m_context_data;

   private:
    void create_instance();
    void setup_debug_messenger();
    void create_surface(EspWindow& window);
    void pick_physical_device();
    void create_logical_device();

   private:
    void create_buffer_command_object();
    void create_frame_scheduler();

   public:
    VulkanContext(const VulkanContext& other) = delete;

    VulkanContext& operator=(const VulkanContext& other) = delete;

   public:
    /* ---------------- API ------------------------------------------- */
    virtual ~VulkanContext() override;

    void init(EspWindow& window) override;
    void terminate() override;
    void update() override;

    static std::unique_ptr<VulkanContext> create();
    static const ContextData& get_context_data();
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_CONTEXT_HH
