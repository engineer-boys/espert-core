#include "VulkanContext.hh"
#include "GLFW/glfw3.h"
#include "Platform/Vulkan/Resources/VulkanSampler.hh"
#include "Platform/Vulkan/VulkanDebugMessenger.hh"

// std
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

// helpful data type
using ContextData       = esp::VulkanContext::ContextData;
using DeviceContextData = esp::DeviceContextData;

// signatures
static bool check_validation_layer_support(ContextData& context_data);
static std::vector<const char*> get_required_extensions(ContextData& context_data);
static void has_glfw_required_instance_extensions(ContextData& context_data);
static bool is_device_suitable(VkPhysicalDevice device, ContextData& context_data);
static esp::SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device, ContextData& context_data);
static esp::QueueFamilyIndices find_queue_families(VkPhysicalDevice device, ContextData& context_data);
static bool check_device_extension_support(VkPhysicalDevice device, ContextData& context_data);
static void pick_physical_device(ContextData& context_data, DeviceContextData& device_context_data);
static void create_logical_device(ContextData& context_data, DeviceContextData& device_context_data);
static VkSampleCountFlagBits get_max_usable_sample_count();

/* --------------------------------------------------------- */
/* ---------------- CLASS IMPLEMENTATION ------------------- */
/* --------------------------------------------------------- */

namespace esp
{
  VulkanContext* VulkanContext::s_instance = nullptr;

  VulkanContext::VulkanContext()
  {
    ESP_ASSERT(s_instance == nullptr, "The vulkan context already exists!");
    VulkanContext::s_instance = this;

#ifdef __APPLE__
    m_context_data.m_instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    m_context_data.m_instance_extensions.push_back("VK_KHR_get_physical_device_properties2");
    m_context_data.m_device_extensions.push_back("VK_KHR_portability_subset");
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
    create_vulkan_resource_manager(); // TODO: decide what will be done with this class
  }

  void VulkanContext::terminate()
  {
    ESP_ASSERT(s_instance != nullptr, "You cannot terminate vulkan context because it doesn't exist!");

    VulkanSampler::terminate_default_sampler();
    m_vulkan_resource_manager->terminate();
    m_vulkan_device->terminate();

    vkDestroySurfaceKHR(m_context_data.m_instance, m_context_data.m_surface, nullptr);
    vkDestroyInstance(m_context_data.m_instance, nullptr);
    VulkanContext::s_instance = nullptr;
  }

  std::unique_ptr<VulkanContext> VulkanContext::create()
  {
    return std::unique_ptr<VulkanContext>{ new VulkanContext() };
  }

  const VulkanContext::ContextData& VulkanContext::get_context_data() { return s_instance->m_context_data; }

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

  void VulkanContext::create_vulkan_device()
  {
    DeviceContextData device_context_data{};

    // pick_physical_device - selects graphics device in our system which our
    // application will be using
    pick_physical_device(m_context_data, device_context_data);
    // create_logical_device - describes what features of our physical device we
    // want to use
    create_logical_device(m_context_data, device_context_data);

    m_vulkan_device = VulkanDevice::create(device_context_data.m_physical_device,
                                           device_context_data.m_device,
                                           device_context_data.m_properties);

    m_context_data.m_msaa_samples = get_max_usable_sample_count();
  }

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

static bool check_validation_layer_support(ContextData& context_data)
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

static std::vector<const char*> get_required_extensions(ContextData& context_data)
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

static void has_glfw_required_instance_extensions(ContextData& context_data)
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

static bool is_device_suitable(VkPhysicalDevice device, ContextData& context_data)
{
  esp::QueueFamilyIndices indices = find_queue_families(device, context_data);

  bool extensions_supported = check_device_extension_support(device, context_data);

  bool swap_chain_adequate = false;
  if (extensions_supported)
  {
    context_data.m_swap_chain_support_details = query_swap_chain_support(device, context_data);
    swap_chain_adequate                       = !context_data.m_swap_chain_support_details.m_formats.empty() &&
        !context_data.m_swap_chain_support_details.m_present_modes.empty();
  }

  VkPhysicalDeviceFeatures supported_features;
  vkGetPhysicalDeviceFeatures(device, &supported_features);

  return indices.is_complete() && extensions_supported && swap_chain_adequate && supported_features.samplerAnisotropy;
}

static esp::SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device, ContextData& context_data)
{
  esp::SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, context_data.m_surface, &details.m_capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, context_data.m_surface, &format_count, nullptr);

  if (format_count != 0)
  {
    details.m_formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, context_data.m_surface, &format_count, details.m_formats.data());
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, context_data.m_surface, &present_mode_count, nullptr);

  if (present_mode_count != 0)
  {
    details.m_present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                              context_data.m_surface,
                                              &present_mode_count,
                                              details.m_present_modes.data());
  }
  return details;
}

static esp::QueueFamilyIndices find_queue_families(VkPhysicalDevice device, ContextData& context_data)
{
  esp::QueueFamilyIndices indices;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  int i = 0;
  for (const auto& queue_family : queue_families)
  {
    if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      indices.m_graphics_family           = i;
      indices.m_graphics_family_has_value = true;
    }
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, context_data.m_surface, &present_support);
    if (queue_family.queueCount > 0 && present_support)
    {
      indices.m_present_family           = i;
      indices.m_present_family_has_value = true;
    }
    if (indices.is_complete()) { break; }

    i++;
  }

  return indices;
}

static bool check_device_extension_support(VkPhysicalDevice device, ContextData& context_data)
{
  uint32_t extensions_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, nullptr);

  std::vector<VkExtensionProperties> available_extensions(extensions_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, available_extensions.data());

  std::set<std::string> required_extensions(context_data.m_device_extensions.begin(),
                                            context_data.m_device_extensions.end());

  for (const auto& extension : available_extensions)
  {
    required_extensions.erase(extension.extensionName);
  }

  return required_extensions.empty();
}

static void pick_physical_device(ContextData& context_data, DeviceContextData& device_context_data)
{
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(context_data.m_instance, &device_count, nullptr);
  if (device_count == 0)
  {
    ESP_CORE_ERROR("Failed to find GPUs with Vulkan support");
    throw std::runtime_error("Failed to find GPUs with Vulkan support");
  }
  ESP_CORE_INFO("Device count: {0}", device_count);
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(context_data.m_instance, &device_count, devices.data());

  for (const auto& device : devices)
  {
    if (is_device_suitable(device, context_data))
    {
      device_context_data.m_physical_device = device;
      break;
    }
  }

  if (device_context_data.m_physical_device == VK_NULL_HANDLE)
  {
    ESP_CORE_ERROR("Failed to find a suitable GPU");
    throw std::runtime_error("Failed to find a suitable GPU");
  }

  context_data.m_queue_family_indices = find_queue_families(device_context_data.m_physical_device, context_data);

  vkGetPhysicalDeviceProperties(device_context_data.m_physical_device, &device_context_data.m_properties);
  ESP_CORE_INFO("Physical device: {0}", device_context_data.m_properties.deviceName);
}

static void create_logical_device(ContextData& context_data, DeviceContextData& device_context_data)
{
  esp::QueueFamilyIndices& indices = context_data.m_queue_family_indices;

  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = { indices.m_graphics_family, indices.m_present_family };

  float queue_priority = 1.0f;
  for (uint32_t queue_family : unique_queue_families)
  {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex        = queue_family;
    queue_create_info.queueCount              = 1;
    queue_create_info.pQueuePriorities        = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  VkPhysicalDeviceFeatures device_features = {};
  device_features.samplerAnisotropy        = VK_TRUE;
  // TODO: let user decide whether he wants higher quality or better performance - put this in some if statement
  // device_features.sampleRateShading        = VK_TRUE; // enable sample shading feature for the device

  VkDeviceCreateInfo create_info = {};
  create_info.sType              = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
  create_info.pQueueCreateInfos    = queue_create_infos.data();

  create_info.pEnabledFeatures        = &device_features;
  create_info.enabledExtensionCount   = static_cast<uint32_t>(context_data.m_device_extensions.size());
  create_info.ppEnabledExtensionNames = context_data.m_device_extensions.data();

  // might not really be necessary anymore because device specific validation
  // layers have been deprecated
  if (context_data.m_enable_validation_layers)
  {
    create_info.enabledLayerCount   = static_cast<uint32_t>(context_data.m_validation_layers.size());
    create_info.ppEnabledLayerNames = context_data.m_validation_layers.data();
  }
  else { create_info.enabledLayerCount = 0; }

  if (vkCreateDevice(device_context_data.m_physical_device, &create_info, nullptr, &device_context_data.m_device) !=
      VK_SUCCESS)
  {
    ESP_CORE_ERROR("Failed to create logical device");
    throw std::runtime_error("Failed to create logical device");
  }
  volkLoadDevice(device_context_data.m_device);

  vkGetDeviceQueue(device_context_data.m_device, indices.m_graphics_family, 0, &context_data.m_graphics_queue);
  vkGetDeviceQueue(device_context_data.m_device, indices.m_present_family, 0, &context_data.m_present_queue);
}

static VkSampleCountFlagBits get_max_usable_sample_count()
{
  VkPhysicalDeviceProperties properties = esp::VulkanDevice::get_properties();

  VkSampleCountFlags counts =
      properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
  if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
  if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
  if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
  if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
  if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
  if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

  return VK_SAMPLE_COUNT_1_BIT;
}