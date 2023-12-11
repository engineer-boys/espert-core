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

    VkBool32 Log(const VkDebugUtilsMessageSeverityFlagBitsEXT& messageSeverity,
                 VkDebugUtilsMessageTypeFlagsEXT messageType,
                 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);

   public:
    virtual ~VulkanDebugMessenger() override;
    PREVENT_COPY(VulkanDebugMessenger);

    static std::unique_ptr<VulkanDebugMessenger> create();
    virtual void init() override;
    virtual void terminate() override;

    static VKAPI_ATTR VkBool32 VKAPI_CALL Log(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                              void* pUserData);

    inline static const VkDebugUtilsMessengerCreateInfoEXT* get_create_info()
    {
      return &(dynamic_cast<VulkanDebugMessenger*>(s_instance)->m_create_info);
    }
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_DEBUG_MESSENGER_HH