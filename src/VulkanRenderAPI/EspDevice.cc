#include "EspDevice.hh"

// std
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

namespace esp
{
  // local callback functions
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                 VkDebugUtilsMessageTypeFlagsEXT message_type,
                 const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
                 void* p_user_data)
  {
    ESP_CORE_ERROR("Validation layer: {0}", p_callback_data->pMessage);

    return VK_FALSE;
  }

  VkResult create_debug_utils_messenger_EXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
      const VkAllocationCallbacks* p_allocator,
      VkDebugUtilsMessengerEXT* p_debug_messenger)
  {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
      return func(instance, p_create_info, p_allocator, p_debug_messenger);
    }
    else { return VK_ERROR_EXTENSION_NOT_PRESENT; }
  }

  void
  destroy_debug_utils_messenger_EXT(VkInstance instance,
                                    VkDebugUtilsMessengerEXT debugMessenger,
                                    const VkAllocationCallbacks* pAllocator)
  {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) { func(instance, debugMessenger, pAllocator); }
  }

  // class member functions
  EspDevice::EspDevice(EspWindow& window) : m_window{ window }
  {
    // create_instance - initializes vulkan library, it's connection between our
    // application and vulkan
    create_instance();
    // setup_debug_messenger - enables validation layers for debugging
    setup_debug_messenger();
    // create_surface - connection between our GLFWwindow and vulkan's ability
    // do display results
    create_surface();
    // pick_physical_device - selects graphics device in our system which our
    // application will be using
    pick_physical_device();
    // create_logical_device - describes what features of our physical device we
    // want to use
    create_logical_device();
    // create_command_pool - allocate command buffer etc.
    create_command_pool();
  }

  EspDevice::~EspDevice()
  {
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
    vkDestroyDevice(m_device, nullptr);

    if (m_enable_validation_layers)
    {
      destroy_debug_utils_messenger_EXT(m_instance, m_debug_messenger, nullptr);
    }

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);
  }

  void EspDevice::create_instance()
  {
    if (volkInitialize() != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to initialize volk");
      throw std::runtime_error("Failed to initialize volk");
    }

    if (m_enable_validation_layers && !check_validation_layer_support())
    {
      ESP_CORE_ERROR("Validation layers requested, but not available");
      throw std::runtime_error(
          "Validation layers requested, but not available");
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

    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    auto extensions = get_required_extensions();
    create_info.enabledExtensionCount =
        static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if (m_enable_validation_layers)
    {
      create_info.enabledLayerCount =
          static_cast<uint32_t>(m_validation_layers.size());
      create_info.ppEnabledLayerNames = m_validation_layers.data();

      populate_debug_messenger_create_info(debug_create_info);
      create_info.pNext =
          (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    }
    else
    {
      create_info.enabledLayerCount = 0;
      create_info.pNext             = nullptr;
    }

    if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create instance");
      throw std::runtime_error("Failed to create instance");
    }
    volkLoadInstance(m_instance);

    has_glfw_required_instance_extensions();
  }

  void EspDevice::pick_physical_device()
  {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
    if (device_count == 0)
    {
      ESP_CORE_ERROR("Failed to find GPUs with Vulkan support");
      throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }
    ESP_CORE_INFO("Device count: {0}", device_count);
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

    for (const auto& device : devices)
    {
      if (is_device_suitable(device))
      {
        m_physical_device = device;
        break;
      }
    }

    if (m_physical_device == VK_NULL_HANDLE)
    {
      ESP_CORE_ERROR("Failed to find a suitable GPU");
      throw std::runtime_error("Failed to find a suitable GPU");
    }

    vkGetPhysicalDeviceProperties(m_physical_device, &m_properties);
    ESP_CORE_INFO("Physical device: {0}", m_properties.deviceName);
  }

  void EspDevice::create_logical_device()
  {
    QueueFamilyIndices indices = find_queue_families(m_physical_device);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = { indices.m_graphics_family,
                                                 indices.m_present_family };

    float queue_priority = 1.0f;
    for (uint32_t queue_family : unique_queue_families)
    {
      VkDeviceQueueCreateInfo queue_create_info = {};
      queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_create_info.queueFamilyIndex = queue_family;
      queue_create_info.queueCount       = 1;
      queue_create_info.pQueuePriorities = &queue_priority;
      queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy        = VK_TRUE;

    VkDeviceCreateInfo create_info = {};
    create_info.sType              = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.queueCreateInfoCount =
        static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();

    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount =
        static_cast<uint32_t>(m_device_extensions.size());
    create_info.ppEnabledExtensionNames = m_device_extensions.data();

    // might not really be necessary anymore because device specific validation
    // layers have been deprecated
    if (m_enable_validation_layers)
    {
      create_info.enabledLayerCount =
          static_cast<uint32_t>(m_validation_layers.size());
      create_info.ppEnabledLayerNames = m_validation_layers.data();
    }
    else { create_info.enabledLayerCount = 0; }

    if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) !=
        VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create logical device");
      throw std::runtime_error("Failed to create logical device");
    }
    volkLoadDevice(m_device);

    vkGetDeviceQueue(m_device, indices.m_graphics_family, 0, &m_graphics_queue);
    vkGetDeviceQueue(m_device, indices.m_present_family, 0, &m_present_queue);
  }

  void EspDevice::create_command_pool()
  {
    QueueFamilyIndices queue_family_indices = find_physical_queue_families();

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_family_indices.m_graphics_family;
    pool_info.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool) !=
        VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create command pool");
      throw std::runtime_error("Failed to create command pool");
    }
  }

  void EspDevice::create_surface()
  {
    m_window.create_window_surface(m_instance, &m_surface);
  }

  bool EspDevice::is_device_suitable(VkPhysicalDevice device)
  {
    QueueFamilyIndices indices = find_queue_families(device);

    bool extensions_supported = check_device_extension_support(device);

    bool swap_chain_adequate = false;
    if (extensions_supported)
    {
      SwapChainSupportDetails swap_chain_support =
          query_swap_chain_support(device);
      swap_chain_adequate = !swap_chain_support.m_formats.empty() &&
          !swap_chain_support.m_present_modes.empty();
    }

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(device, &supported_features);

    return indices.isComplete() && extensions_supported &&
        swap_chain_adequate && supported_features.samplerAnisotropy;
  }

  void EspDevice::populate_debug_messenger_create_info(
      VkDebugUtilsMessengerCreateInfoEXT& create_info)
  {
    create_info       = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    create_info.pUserData       = nullptr; // Optional
  }

  void EspDevice::setup_debug_messenger()
  {
    if (!m_enable_validation_layers) return;
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_messenger_create_info(create_info);
    if (create_debug_utils_messenger_EXT(m_instance, &create_info, nullptr,
                                         &m_debug_messenger) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to set up debug messenger");
      throw std::runtime_error("Failed to set up debug messenger");
    }
  }

  bool EspDevice::check_validation_layer_support()
  {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char* layer_name : m_validation_layers)
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

  std::vector<const char*> EspDevice::get_required_extensions()
  {
    uint32_t glfw_extensions_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

    std::vector<const char*> extensions(
        glfw_extensions, glfw_extensions + glfw_extensions_count);

    for (auto instance_extension : m_instance_extensions)
    {
      extensions.emplace_back(instance_extension);
    }

    if (m_enable_validation_layers)
    {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  void EspDevice::has_glfw_required_instance_extensions()
  {
    uint32_t extensions_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extensions_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count,
                                           extensions.data());

    ESP_CORE_INFO("Available extensions:");
    std::unordered_set<std::string> available;
    for (const auto& extension : extensions)
    {
      std::cout << "\t" << extension.extensionName << std::endl;
      available.insert(extension.extensionName);
    }

    ESP_CORE_INFO("Required extensions:");
    auto requiredExtensions = get_required_extensions();
    for (const auto& required : requiredExtensions)
    {
      std::cout << "\t" << required << std::endl;
      if (available.find(required) == available.end())
      {
        ESP_CORE_ERROR("Missing required glfw extension");
        throw std::runtime_error("Missing required glfw extension");
      }
    }
  }

  bool EspDevice::check_device_extension_support(VkPhysicalDevice device)
  {
    uint32_t extensions_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count,
                                         nullptr);

    std::vector<VkExtensionProperties> available_extensions(extensions_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count,
                                         available_extensions.data());

    std::set<std::string> required_extensions(m_device_extensions.begin(),
                                              m_device_extensions.end());

    for (const auto& extension : available_extensions)
    {
      required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
  }

  QueueFamilyIndices EspDevice::find_queue_families(VkPhysicalDevice device)
  {
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             queue_families.data());

    int i = 0;
    for (const auto& queue_family : queue_families)
    {
      if (queue_family.queueCount > 0 &&
          queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      {
        indices.m_graphics_family           = i;
        indices.m_graphics_family_has_value = true;
      }
      VkBool32 present_support = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface,
                                           &present_support);
      if (queue_family.queueCount > 0 && present_support)
      {
        indices.m_present_family           = i;
        indices.m_present_family_has_value = true;
      }
      if (indices.isComplete()) { break; }

      i++;
    }

    return indices;
  }

  SwapChainSupportDetails
  EspDevice::query_swap_chain_support(VkPhysicalDevice device)
  {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface,
                                              &details.m_capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count,
                                         nullptr);

    if (format_count != 0)
    {
      details.m_formats.resize(format_count);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count,
                                           details.m_formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface,
                                              &present_mode_count, nullptr);

    if (present_mode_count != 0)
    {
      details.m_present_modes.resize(present_mode_count);
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface,
                                                &present_mode_count,
                                                details.m_present_modes.data());
    }
    return details;
  }

  VkFormat
  EspDevice::find_supported_format(const std::vector<VkFormat>& candidates,
                                   VkImageTiling tiling,
                                   VkFormatFeatureFlags features)
  {
    for (VkFormat format : candidates)
    {
      VkFormatProperties props;
      vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

      if (tiling == VK_IMAGE_TILING_LINEAR &&
          (props.linearTilingFeatures & features) == features)
      {
        return format;
      }
      else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
               (props.optimalTilingFeatures & features) == features)
      {
        return format;
      }
    }

    ESP_CORE_ERROR("Failed to find supported format");
    throw std::runtime_error("Failed to find supported format");
  }

  uint32_t EspDevice::find_memory_type(uint32_t type_filter,
                                       VkMemoryPropertyFlags properties)
  {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(m_physical_device, &mem_properties);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
    {
      if ((type_filter & (1 << i)) &&
          (mem_properties.memoryTypes[i].propertyFlags & properties) ==
              properties)
      {
        return i;
      }
    }

    ESP_CORE_ERROR("Failed to find suitable memory type");
    throw std::runtime_error("Failed to find suitable memory type");
  }

  void EspDevice::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkBuffer& buffer, VkDeviceMemory& buffer_memory)
  {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size        = size;
    buffer_info.usage       = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create vertex buffer");
      throw std::runtime_error("Failed to create vertex buffer");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(m_device, buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex =
        find_memory_type(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device, &alloc_info, nullptr, &buffer_memory) !=
        VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate vertex buffer memory");
      throw std::runtime_error("Failed to allocate vertex buffer memory");
    }

    vkBindBufferMemory(m_device, buffer, buffer_memory, 0);
  }

  VkCommandBuffer EspDevice::begin_single_time_commands()
  {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(m_device, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);
    return command_buffer;
  }

  void EspDevice::end_single_time_commands(VkCommandBuffer command_buffer)
  {
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = &command_buffer;

    vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphics_queue);

    vkFreeCommandBuffers(m_device, m_command_pool, 1, &command_buffer);
  }

  void EspDevice::copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer,
                              VkDeviceSize size)
  {
    VkCommandBuffer command_buffer = begin_single_time_commands();

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0; // Optional
    copy_region.dstOffset = 0; // Optional
    copy_region.size      = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    end_single_time_commands(command_buffer);
  }

  void EspDevice::copy_buffer_to_image(VkBuffer buffer, VkImage image,
                                       uint32_t width, uint32_t height,
                                       uint32_t layer_count)
  {
    VkCommandBuffer command_buffer = begin_single_time_commands();

    VkBufferImageCopy region{};
    region.bufferOffset      = 0;
    region.bufferRowLength   = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = layer_count;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(command_buffer, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    end_single_time_commands(command_buffer);
  }

  void EspDevice::create_image_with_info(const VkImageCreateInfo& image_info,
                                         VkMemoryPropertyFlags properties,
                                         VkImage& image,
                                         VkDeviceMemory& image_memory)
  {
    if (vkCreateImage(m_device, &image_info, nullptr, &image) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create image");
      throw std::runtime_error("Failed to create image");
    }

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(m_device, image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex =
        find_memory_type(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device, &alloc_info, nullptr, &image_memory) !=
        VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate image memory");
      throw std::runtime_error("Failed to allocate image memory");
    }

    if (vkBindImageMemory(m_device, image, image_memory, 0) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to bind image memory");
      throw std::runtime_error("Failed to bind image memory");
    }
  }
} // namespace esp