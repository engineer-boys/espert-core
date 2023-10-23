#ifndef ESPERT_PLATFORMS_VULKANMINIRAPI_VULKANMINIRCONTEXT_HH_
#define ESPERT_PLATFORMS_VULKANMINIRAPI_VULKANMINIRCONTEXT_HH_

#include "Core/MiniRAPI/MiniRContext.hh"
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

    bool isComplete() const
    {
      return m_graphics_family_has_value && m_present_family_has_value;
    }
  };

  class VulkanMiniRAPIContext : public MiniRContext
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

      const std::vector<const char*> m_validation_layers = {
        "VK_LAYER_KHRONOS_validation"
      };

      std::vector<const char*> m_instance_extensions = {};
      std::vector<const char*> m_device_extensions   = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      };

      const bool m_enable_validation_layers = true;
    };

   private:
    static VulkanMiniRAPIContext* s_instance;
    VulkanMiniRAPIContext();

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
    VulkanMiniRAPIContext(const VulkanMiniRAPIContext& other) = delete;

    VulkanMiniRAPIContext&
    operator=(const VulkanMiniRAPIContext& other) = delete;

   public:
    /* ---------------- API ------------------------------------------- */
    virtual ~VulkanMiniRAPIContext() override;

    void init(EspWindow& window);
    void terminate();
    void update();

    static std::unique_ptr<VulkanMiniRAPIContext> create();
    static const ContextData& get_context_data();
  };
} // namespace esp

#endif /* ESPERT_PLATFORMS_VULKANMINIRAPI_VULKANMINIRCONTEXT_HH_ */