#include "VulkanContext.hh"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "Platform/Vulkan/Resources/VulkanSampler.hh"
#include "Platform/Vulkan/VulkanDebugMessenger.hh"
#include "Platform/Vulkan/Work/VulkanSwapChain.hh"

// std
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

// signatures
static bool check_validation_layer_support(esp::VulkanContextData& context_data);
static std::vector<const char*> get_required_extensions(esp::VulkanContextData& context_data);
static void has_glfw_required_instance_extensions(esp::VulkanContextData& context_data);

/* --------------------------------------------------------- */
/* ---------------- CLASS IMPLEMENTATION ------------------- */
/* --------------------------------------------------------- */

namespace esp
{
  VulkanContext* VulkanContext::s_instance = nullptr;

  std::unique_ptr<VulkanContext> VulkanContext::create(EspWindow& window)
  {
    ESP_ASSERT(VulkanContext::s_instance == nullptr, "The vulkan context already exists!");
    VulkanContext::s_instance = new VulkanContext();
    VulkanContext::s_instance->init(window);

    return std::unique_ptr<VulkanContext>{ VulkanContext::s_instance };
  }

  VulkanContext::VulkanContext()
  {
#ifdef __APPLE__
    m_context_data.m_instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif /* __APPLE__ */
  }

  void VulkanContext::init(EspWindow& window)
  {
    // create_instance - initializes vulkan library, it's connection between our
    // application and vulkan
    create_instance();
    // create_surface - connection between our GLFWwindow and vulkan's ability
    // do display results
    create_surface(window);
    // create_vulkan_device - wraps VkPhysicalDevice and VkDevice into a single unit
    create_vulkan_device();

    create_default_sampler();
    create_vulkan_resource_manager();
  }

  void VulkanContext::terminate()
  {
    ESP_ASSERT(s_instance != nullptr, "You cannot terminate vulkan context because it doesn't exist!");

    // it only depends on logical device
    {
      VulkanSampler::terminate_default_sampler(); // TODO: move this to vulkan resource manager!!!
      m_vulkan_resource_manager->terminate();
    }

    m_vulkan_device->terminate();

    vkDestroySurfaceKHR(m_context_data.m_instance, m_context_data.m_surface, nullptr);
    vkDestroyInstance(m_context_data.m_instance, nullptr);

    VulkanContext::s_instance = nullptr;
  }

  /* ------------------ API DEPENDENT CODE ------------------- */

  void VulkanContext::create_instance()
  {
    if (volkInitialize() != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to initialize volk");
      throw std::runtime_error("Failed to initialize volk");
    }

    if (m_context_data.m_enable_validation_layers && !check_validation_layer_support(m_context_data))
    {
      ESP_CORE_ERROR("Validation layers requested, but not available");
      throw std::runtime_error("Validation layers requested, but not available");
    }

    VkApplicationInfo app_info  = {};
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName   = "Espert app";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName        = "Espert";
    app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion         = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info = {};
    create_info.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo     = &app_info;

#ifdef __APPLE__
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif /* __APPLE__ */

    auto extensions                     = get_required_extensions(m_context_data);
    create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    if (m_context_data.m_enable_validation_layers)
    {
      create_info.enabledLayerCount   = static_cast<uint32_t>(m_context_data.m_validation_layers.size());
      create_info.ppEnabledLayerNames = m_context_data.m_validation_layers.data();

      create_info.pNext = VulkanDebugMessenger::get_create_info();
    }
    else
    {
      create_info.enabledLayerCount = 0;
      create_info.pNext             = nullptr;
    }

    if (vkCreateInstance(&create_info, nullptr, &(m_context_data.m_instance)) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create instance");
      throw std::runtime_error("Failed to create instance");
    }
    volkLoadInstance(m_context_data.m_instance);

    has_glfw_required_instance_extensions(m_context_data);
  }

  void VulkanContext::create_surface(EspWindow& window) { window.create_window_surface(); }

  void VulkanContext::create_vulkan_device() { m_vulkan_device = VulkanDevice::create(&m_context_data); }

  void VulkanContext::create_default_sampler() { VulkanSampler::create_default_sampler(); }

  void VulkanContext::create_vulkan_resource_manager() { m_vulkan_resource_manager = VulkanResourceManager::create(); }

  void render_context_glfw_hints()
  {
    ESP_CORE_INFO("GLFW hints for Vulkan");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  }
} // namespace esp

/* --------------------------------------------------------- */
/* ------------------ HELPFUL FUNCTIONS -------------------- */
/* --------------------------------------------------------- */

static bool check_validation_layer_support(esp::VulkanContextData& context_data)
{
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char* layer_name : context_data.m_validation_layers)
  {
    bool layer_found = false;

    for (const auto& layer_properties : available_layers)
    {
      if (strcmp(layer_name, layer_properties.layerName) == 0)
      {
        layer_found = true;
        break;
      }
    }

    if (!layer_found) { return false; }
  }

  return true;
}

static std::vector<const char*> get_required_extensions(esp::VulkanContextData& context_data)
{
  uint32_t glfw_extensions_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

  std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extensions_count);

  for (auto instance_extension : context_data.m_instance_extensions)
  {
    extensions.emplace_back(instance_extension);
  }

  if (context_data.m_enable_validation_layers) { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }

  return extensions;
}

static void has_glfw_required_instance_extensions(esp::VulkanContextData& context_data)
{
  uint32_t extensions_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
  std::vector<VkExtensionProperties> extensions(extensions_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, extensions.data());

  ESP_CORE_INFO("Available extensions:");
  std::unordered_set<std::string> available;
  for (const auto& extension : extensions)
  {
    std::cout << "\t" << extension.extensionName << std::endl;
    available.insert(extension.extensionName);
  }

  ESP_CORE_INFO("Required extensions:");
  auto required_extensions = get_required_extensions(context_data);
  for (const auto& required : required_extensions)
  {
    std::cout << "\t" << required << std::endl;
    if (available.find(required) == available.end())
    {
      ESP_CORE_ERROR("Missing required glfw extension");
      throw std::runtime_error("Missing required glfw extension");
    }
  }
}