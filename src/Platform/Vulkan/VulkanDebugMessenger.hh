#ifndef VULKAN_RENDER_API_VULKAN_DEBUG_MESSENGER_HH
#define VULKAN_RENDER_API_VULKAN_DEBUG_MESSENGER_HH

#include "esppch.hh"

#include "Core/RenderAPI/EspDebugMessenger.hh"

namespace esp
{
  /// @brief Responsible for logging and counting Vulkan's debug messages.
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
    /// @brief Virtual destructor.
    virtual ~VulkanDebugMessenger() override;
    PREVENT_COPY(VulkanDebugMessenger);

    /// @brief Creates instance of VulkanDebugMessenger.
    /// @return Unique pointer to instance of VulkanDebugMessenger.
    static std::unique_ptr<VulkanDebugMessenger> create();
    /// @brief Initializes the debug messenger.
    virtual void init() override;
    /// @brief Termiantes the debug messenger.
    virtual void terminate() override;

    /// @brief Callback for Vulkan's internal debug messenger.
    /// @param message_severity Severity of the message.
    /// @param message_type Type of the message.
    /// @param p_callback_data Extra data attached to the message.
    /// @param p_user_data Extra user data attached to the call.
    /// @return VK_FALSE if message was correctly handled.
    static VKAPI_ATTR VkBool32 VKAPI_CALL log(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                              VkDebugUtilsMessageTypeFlagsEXT message_type,
                                              const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
                                              void* p_user_data);

    /// @brief Returns Vulkan's internal debug messenger create info.
    /// @return Vulkan's internal debug messenger create info.
    inline static const VkDebugUtilsMessengerCreateInfoEXT* get_create_info()
    {
      return &(dynamic_cast<VulkanDebugMessenger*>(s_instance)->m_create_info);
    }
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_DEBUG_MESSENGER_HH