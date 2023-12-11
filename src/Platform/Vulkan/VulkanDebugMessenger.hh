#ifndef VULKAN_RENDER_API_VULKAN_DEBUG_MESSENGER_HH
#define VULKAN_RENDER_API_VULKAN_DEBUG_MESSENGER_HH

#include "esppch.hh"

#include "Core/RenderAPI/EspDebugMessenger.hh"

namespace esp
{
  class VulkanDebugMessenger : public EspDebugMessenger
  {
   private:
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VkDebugUtilsMessengerCreateInfoEXT m_create_info;

   private:
    VulkanDebugMessenger();

    VkBool32 log(const VkDebugUtilsMessageSeverityFlagBitsEXT& message_severity,
                 VkDebugUtilsMessageTypeFlagsEXT message_type,
                 const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data);

   public:
    virtual ~VulkanDebugMessenger() override;
    PREVENT_COPY(VulkanDebugMessenger);

    static std::unique_ptr<VulkanDebugMessenger> create();
    virtual void init() override;
    virtual void terminate() override;

    static VKAPI_ATTR VkBool32 VKAPI_CALL log(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                              VkDebugUtilsMessageTypeFlagsEXT message_type,
                                              const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
                                              void* p_user_data);

    inline static const VkDebugUtilsMessengerCreateInfoEXT* get_create_info()
    {
      return &(dynamic_cast<VulkanDebugMessenger*>(s_instance)->m_create_info);
    }
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_DEBUG_MESSENGER_HH