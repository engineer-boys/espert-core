#include "VulkanSwapChain.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

// std
#include <array>
#include <cstdlib>
#include <limits>
#include <set>
#include <stdexcept>

// signatures
static VkFormat find_depth_format();
static VkSurfaceFormatKHR choose_swap_chain_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
static VkPresentModeKHR choose_swap_chain_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);
static VkExtent2D choose_swap_chain_extent(const VkSurfaceCapabilitiesKHR& capabilities);
static void log_swap_chain_image_format(VkFormat format);
// static void log_chosen_swap_chain_present_mode(const std::string& mode);

/* --------------------------------------------------------- */
/* ---------------- CLASS IMPLEMENTATION ------------------- */
/* --------------------------------------------------------- */

namespace esp
{
  VulkanSwapChain* VulkanSwapChain::s_instance = nullptr;

  std::unique_ptr<VulkanSwapChain> VulkanSwapChain::create()
  {
    ESP_ASSERT(VulkanSwapChain::s_instance == nullptr, "VulkanSwapChain already exists!");
    VulkanSwapChain::s_instance = new VulkanSwapChain();
    VulkanSwapChain::s_instance->init();

    return std::unique_ptr<VulkanSwapChain>{ VulkanSwapChain::s_instance };
  }

  VulkanSwapChain::VulkanSwapChain() {}

  VulkanSwapChain::~VulkanSwapChain() {}

  void VulkanSwapChain::init() { create_swap_chain(VK_NULL_HANDLE); }

  void VulkanSwapChain::terminate()
  {
    ESP_ASSERT(VulkanSwapChain::s_instance != nullptr, "VulkanSwapChain is deleted twice!");

    for (auto buffer : m_swap_chain_buffers)
    {
      buffer.terminate();
    }
    m_swap_chain_buffers.clear();

    if (m_swap_chain != nullptr)
    {
      vkDestroySwapchainKHR(VulkanDevice::get_logical_device(), m_swap_chain, nullptr);
      m_swap_chain = nullptr;
    }

    VulkanSwapChain::s_instance = nullptr;
  }

  void VulkanSwapChain::create_swap_chain(VkSwapchainKHR old_swap_chain)
  {
    auto& context_data = VulkanContext::get_context_data();

    SwapChainSupportDetails swap_chain_support =
        VulkanSwapChain::query_swap_chain_support(VulkanDevice::get_physical_device(), context_data);

    VkSurfaceFormatKHR surface_format = choose_swap_chain_surface_format(swap_chain_support.m_formats);
    VkPresentModeKHR present_mode     = choose_swap_chain_present_mode(swap_chain_support.m_present_modes);
    VkExtent2D extent                 = choose_swap_chain_extent(swap_chain_support.m_capabilities);

    uint32_t image_count = swap_chain_support.m_capabilities.minImageCount + 1;
    if (swap_chain_support.m_capabilities.maxImageCount > 0 &&
        image_count > swap_chain_support.m_capabilities.maxImageCount)
    {
      image_count = swap_chain_support.m_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface                  = context_data.m_surface;
    create_info.minImageCount            = image_count;
    create_info.imageFormat              = surface_format.format;
    create_info.imageColorSpace          = surface_format.colorSpace;
    create_info.imageExtent              = extent;
    create_info.imageArrayLayers         = 1;
    create_info.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices      = context_data.m_queue_family_indices;
    uint32_t queue_family_indices[] = { indices.m_graphics_family.value(), indices.m_present_family.value() };

    if (indices.m_graphics_family != indices.m_present_family)
    {
      create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
      create_info.queueFamilyIndexCount = 2;
      create_info.pQueueFamilyIndices   = queue_family_indices;
    }
    else
    {
      create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
      create_info.queueFamilyIndexCount = 0;       // Optional
      create_info.pQueueFamilyIndices   = nullptr; // Optional
    }

    create_info.preTransform   = swap_chain_support.m_capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    create_info.presentMode = present_mode;
    create_info.clipped     = VK_TRUE;

    create_info.oldSwapchain = old_swap_chain;

    if (vkCreateSwapchainKHR(VulkanDevice::get_logical_device(), &create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create swap chain");
      throw std::runtime_error("Failed to create swap chain");
    }

    // we only specified a minimum number of images in the swap chain, so the
    // implementation is allowed to create a swap chain with more. That's why
    // we'll first query the final number of images with
    // vkGetSwapchainImagesKHR, then resize the container and finally call it
    // again to retrieve the handles.
    std::vector<VkImage> images;
    vkGetSwapchainImagesKHR(VulkanDevice::get_logical_device(), m_swap_chain, &image_count, nullptr);
    images.resize(image_count);
    vkGetSwapchainImagesKHR(VulkanDevice::get_logical_device(), m_swap_chain, &image_count, images.data());

    for (auto image : images)
    {
      m_swap_chain_buffers.emplace_back(image, surface_format.format);
    }

    m_swap_chain_image_format = surface_format.format;
    m_swap_chain_extent       = extent;
  }

  SwapChainSupportDetails VulkanSwapChain::query_swap_chain_support(const VkPhysicalDevice& device,
                                                                    const VulkanContextData& context_data)
  {
    SwapChainSupportDetails details;
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
} // namespace esp

static VkFormat find_depth_format()
{
  return esp::VulkanDevice::find_supported_format(
      { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
      VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

static VkSurfaceFormatKHR choose_swap_chain_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
  std::vector<VkFormat> preferred_image_unorm_formats = { VK_FORMAT_B8G8R8A8_UNORM,
                                                          VK_FORMAT_R8G8B8A8_UNORM,
                                                          VK_FORMAT_A8B8G8R8_UNORM_PACK32 };

  for (const auto& available_format : available_formats)
  {
    for (const auto& prefered_format : preferred_image_unorm_formats)
    {
      if (available_format.format == prefered_format)
      {
        log_swap_chain_image_format(prefered_format);
        return available_format;
      }
    }
  }

  log_swap_chain_image_format(available_formats[0].format);
  return available_formats[0];
}

static VkPresentModeKHR choose_swap_chain_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes)
{
  /*for (const auto& available_present_mode : available_present_modes)
  {
    if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      log_chosen_swap_chain_present_mode("Mailbox");
      return available_present_mode;
    }
  }*/

  // log_chosen_swap_chain_present_mode("V-Sync");
  return VK_PRESENT_MODE_IMMEDIATE_KHR;
}

static VkExtent2D choose_swap_chain_extent(const VkSurfaceCapabilitiesKHR& capabilities)
{
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) { return capabilities.currentExtent; }
  else
  {
    int width, height;
    esp::EspWindow::get_framebuffer_size(width, height);
    VkExtent2D actual_extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

    actual_extent.width =
        std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height =
        std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actual_extent;
  }
}

static void log_swap_chain_image_format(VkFormat format)
{
  if (format == VK_FORMAT_B8G8R8A8_UNORM) { ESP_INFO("Swap chain format : VK_FORMAT_B8G8R8A8_UNORM"); }
  else if (format == VK_FORMAT_R8G8B8A8_UNORM) { ESP_INFO("Swap chain format : VK_FORMAT_R8G8B8A8_UNORM"); }
  else if (format == VK_FORMAT_A8B8G8R8_UNORM_PACK32)
  {
    ESP_INFO("Swap chain format : VK_FORMAT_A8B8G8R8_UNORM_PACK32");
  }
  else { ESP_INFO("Swap chain format : {} nr", format); }
}
