#include "Platform/Vulkan/VulkanDebugMessenger.hh"

#include "Platform/Vulkan/VulkanContext.hh"

static VkResult create_debug_utils_messenger_ext(VkInstance instance,
                                                 const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
                                                 const VkAllocationCallbacks* p_allocator,
                                                 VkDebugUtilsMessengerEXT* p_debug_messenger);

static void destroy_debug_utils_messenger_ext(VkInstance instance,
                                              VkDebugUtilsMessengerEXT debug_messenger,
                                              const VkAllocationCallbacks* p_allocator);

namespace esp
{
  VulkanDebugMessenger::VulkanDebugMessenger()
  {
    m_create_info                 = {};
    m_create_info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    m_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    m_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    m_create_info.pfnUserCallback = VulkanDebugMessenger::Log;
    m_create_info.pUserData       = reinterpret_cast<void*>(s_instance);
  }

  VulkanDebugMessenger::~VulkanDebugMessenger() { terminate(); }

  std::unique_ptr<VulkanDebugMessenger> VulkanDebugMessenger::create()
  {
    auto debug_messenger = std::unique_ptr<VulkanDebugMessenger>(new VulkanDebugMessenger());
    return debug_messenger;
  }

  void VulkanDebugMessenger::init()
  {
    if (create_debug_utils_messenger_ext(VulkanContext::get_context_data().m_instance,
                                         &m_create_info,
                                         nullptr,
                                         &m_debug_messenger) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to set up debug messenger");
      throw std::runtime_error("Failed to set up debug messenger");
    }
  }

  void VulkanDebugMessenger::terminate()
  {
    if (s_instance)
    {
      destroy_debug_utils_messenger_ext(VulkanContext::get_context_data().m_instance, m_debug_messenger, nullptr);
      VulkanDebugMessenger::s_instance = nullptr;
      ESP_CORE_TRACE("Debug messenger terminated.");
    }
  }

  VkBool32 VulkanDebugMessenger::Log(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                     void* pUserData)
  {
    auto pThis = reinterpret_cast<VulkanDebugMessenger*>(pUserData);
    return pThis->Log(messageSeverity, messageType, pCallbackData);
  }

  VkBool32 VulkanDebugMessenger::Log(const VkDebugUtilsMessageSeverityFlagBitsEXT& messageSeverity,
                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
  {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
      ESP_CORE_INFO(pCallbackData->pMessage);
      m_info_count++;
    }
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
      ESP_CORE_WARN(pCallbackData->pMessage);
      m_warning_count++;
    }
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
      ESP_CORE_ERROR(pCallbackData->pMessage);
      m_error_count++;
    }

    return VK_FALSE;
  }
} // namespace esp

static VkResult create_debug_utils_messenger_ext(VkInstance instance,
                                                 const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
                                                 const VkAllocationCallbacks* p_allocator,
                                                 VkDebugUtilsMessengerEXT* p_debug_messenger)
{
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) { return func(instance, p_create_info, p_allocator, p_debug_messenger); }
  else { return VK_ERROR_EXTENSION_NOT_PRESENT; }
}

static void destroy_debug_utils_messenger_ext(VkInstance instance,
                                              VkDebugUtilsMessengerEXT debug_messenger,
                                              const VkAllocationCallbacks* p_allocator)
{
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) { func(instance, debug_messenger, p_allocator); }
}