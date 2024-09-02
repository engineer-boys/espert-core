#include "VulkanDevice.hh"
#include "Platform/Vulkan/Work/VulkanSwapChain.hh"

namespace esp
{
  VulkanDevice* VulkanDevice::s_instance = nullptr;

  std::unique_ptr<VulkanDevice> VulkanDevice::create(VulkanContextData* context_data)
  {
    ESP_ASSERT(VulkanDevice::s_instance == nullptr, "Vulkan device already exists!")

    VulkanDevice::s_instance = new VulkanDevice();
    VulkanDevice::s_instance->init(context_data);

    return std::unique_ptr<VulkanDevice>{ VulkanDevice::s_instance };
  }

  VulkanDevice::VulkanDevice()
  {
#ifdef __APPLE__
    m_device_extensions.push_back("VK_KHR_portability_subset");
#endif /* __APPLE__ */
  }

  VulkanDevice::~VulkanDevice() {}

  void VulkanDevice::init(VulkanContextData* context_data)
  {
    pick_physical_device(context_data);
    create_logical_device(context_data);
  }

  void VulkanDevice::terminate()
  {
    ESP_ASSERT(VulkanDevice::s_instance != nullptr, "VulkanDevice is deleted twice!");

    vkDestroyDevice(m_device, nullptr);

    VulkanDevice::s_instance = nullptr;
  }

  void VulkanDevice::pick_physical_device(VulkanContextData* context_data)
  {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(context_data->m_instance, &device_count, nullptr);
    if (device_count == 0)
    {
      ESP_CORE_ERROR("Failed to find GPUs with Vulkan support");
      throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }
    ESP_CORE_INFO("Device count: {0}", device_count);
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(context_data->m_instance, &device_count, devices.data());

    m_physical_device = VK_NULL_HANDLE;
    for (const auto& device : devices)
    {
      if (is_device_suitable(device, context_data))
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

    context_data->m_queue_family_indices = find_queue_families(m_physical_device, context_data);

    vkGetPhysicalDeviceProperties(m_physical_device, &m_properties);
    ESP_CORE_INFO("Physical device: {0}", m_properties.deviceName);
  }

  void VulkanDevice::create_logical_device(VulkanContextData* context_data)
  {
    esp::QueueFamilyIndices& indices = context_data->m_queue_family_indices;

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = { indices.m_graphics_family.value(), indices.m_present_family.value() };

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
    device_features.fillModeNonSolid         = VK_TRUE;
    device_features.geometryShader           = VK_TRUE;
    device_features.wideLines                = VK_TRUE;
    device_features.tessellationShader       = VK_TRUE;
    // TODO: let user decide whether he wants higher quality or better performance - put this in some if statement
    // device_features.sampleRateShading        = VK_TRUE; // enable sample shading feature for the device

    VkDeviceCreateInfo create_info = {};
    create_info.sType              = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos    = queue_create_infos.data();

    // create_info.pEnabledFeatures        = &device_features;
    // ///////////////////////////////////////////////////////////////////////////////
    create_info.enabledExtensionCount   = static_cast<uint32_t>(m_device_extensions.size());
    create_info.ppEnabledExtensionNames = m_device_extensions.data();

    // might not really be necessary anymore because device specific validation
    // layers have been deprecated
    if (context_data->m_enable_validation_layers)
    {
      create_info.enabledLayerCount   = static_cast<uint32_t>(context_data->m_validation_layers.size());
      create_info.ppEnabledLayerNames = context_data->m_validation_layers.data();
    }
    else { create_info.enabledLayerCount = 0; }

    // this is required to enable dynamic rendering
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature = {};
    dynamic_rendering_feature.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering_feature.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceFeatures2 physical_device_features2{};
    physical_device_features2.sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physical_device_features2.features = device_features;
    physical_device_features2.pNext    = &dynamic_rendering_feature;

    VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT
        physical_device_dynamic_rendering_unused_attachments_features_ext{};
    physical_device_dynamic_rendering_unused_attachments_features_ext.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT;
    physical_device_dynamic_rendering_unused_attachments_features_ext.dynamicRenderingUnusedAttachments = VK_TRUE;
    physical_device_dynamic_rendering_unused_attachments_features_ext.pNext = &physical_device_features2;

    create_info.pEnabledFeatures = nullptr;
    create_info.pNext            = &physical_device_dynamic_rendering_unused_attachments_features_ext;

    if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create logical device");
      throw std::runtime_error("Failed to create logical device");
    }
    volkLoadDevice(m_device);

    vkGetDeviceQueue(m_device, indices.m_graphics_family.value(), 0, &context_data->m_graphics_queue);
    vkGetDeviceQueue(m_device, indices.m_present_family.value(), 0, &context_data->m_present_queue);
    ESP_INFO("Logic degice created");
  }

  bool VulkanDevice::is_device_suitable(VkPhysicalDevice device, VulkanContextData* context_data)
  {
    esp::QueueFamilyIndices indices = find_queue_families(device, context_data);

    bool extensions_supported = check_device_extension_support(device);

    bool swap_chain_adequate = false;
    if (extensions_supported)
    {
      auto swap_chain_support = VulkanSwapChain::query_swap_chain_support(device, *context_data);
      swap_chain_adequate     = !swap_chain_support.m_formats.empty() && !swap_chain_support.m_present_modes.empty();
    }

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(device, &supported_features);

    return indices.is_complete() && extensions_supported && swap_chain_adequate && supported_features.samplerAnisotropy;
  }

  QueueFamilyIndices VulkanDevice::find_queue_families(VkPhysicalDevice device, VulkanContextData* context_data)
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
        indices.m_graphics_family = i;
      }
      VkBool32 present_support = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, context_data->m_surface, &present_support);
      if (queue_family.queueCount > 0 && present_support) { indices.m_present_family = i; }
      if (indices.is_complete()) { break; }

      i++;
    }

    return indices;
  }

  bool VulkanDevice::check_device_extension_support(VkPhysicalDevice device)
  {
    uint32_t extensions_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extensions_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, available_extensions.data());

    std::set<std::string> required_extensions(m_device_extensions.begin(), m_device_extensions.end());

    for (const auto& extension : available_extensions)
    {
      required_extensions.erase(extension.extensionName);
    }

    for (const auto& extension : required_extensions)
    {
      ESP_INFO("you don't have: {0}", extension);
    }

    return required_extensions.empty();
  }

  VkSampleCountFlagBits VulkanDevice::get_max_usable_sample_count()
  {
    VkSampleCountFlags counts =
        m_properties.limits.framebufferColorSampleCounts & m_properties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
  }

  VkFormatProperties VulkanDevice::get_format_properties(VkFormat format)
  {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(s_instance->m_physical_device, format, &properties);
    return properties;
  }

  VkFormat VulkanDevice::find_supported_format(const std::vector<VkFormat>& candidates,
                                               VkImageTiling tiling,
                                               VkFormatFeatureFlags features)
  {
    for (VkFormat format : candidates)
    {
      VkFormatProperties props = get_format_properties(format);

      if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) { return format; }
      if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) { return format; }
    }

    ESP_CORE_ERROR("Failed to find supported format");
    throw std::runtime_error("Failed to find supported format");
  }

  uint32_t VulkanDevice::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties)
  {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(s_instance->m_physical_device, &mem_properties);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
    {
      if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
      {
        return i;
      }
    }

    ESP_CORE_ERROR("Failed to find suitable memory type");
    throw std::runtime_error("Failed to find suitable memory type");
  }
} // namespace esp