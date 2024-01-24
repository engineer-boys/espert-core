#ifndef VULKAN_RENDER_API_VULKAN_SWAP_CHAIN_HH
#define VULKAN_RENDER_API_VULKAN_SWAP_CHAIN_HH

#include "esppch.hh"

#include "Core/EspApplicationParams.hh"

#include "Platform/Vulkan/VulkanContext.hh"

namespace esp
{
  struct SwapChainSupportDetails
  {
    VkSurfaceCapabilitiesKHR m_capabilities;
    std::vector<VkSurfaceFormatKHR> m_formats;
    std::vector<VkPresentModeKHR> m_present_modes;
  };

  struct VulkanSwapChainBuffer
  {
    VkImage m_image;
    VkImageView m_image_view;

    VulkanSwapChainBuffer(VkImage image, VkFormat format) : m_image{ image }
    {
      m_image_view = VulkanResourceManager::create_image_view(m_image, format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    void terminate() { vkDestroyImageView(VulkanDevice::get_logical_device(), m_image_view, nullptr); }
  };

  class VulkanSwapChain
  {
    friend class VulkanWorkOrchestrator;

    /* -------------------------- FIELDS ----------------------------------- */
   private:
    static VulkanSwapChain* s_instance;

    VkSwapchainKHR m_swap_chain;

    VkExtent2D m_swap_chain_extent;
    VkFormat m_swap_chain_image_format;

    std::vector<VulkanSwapChainBuffer> m_swap_chain_buffers;

    uint32_t m_current_frame = 0;
    uint32_t m_image_index   = 0;

   public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    /* -------------------------- METHODS ---------------------------------- */
   private:
    void create_swap_chain(VkSwapchainKHR old_swap_chain, EspPresentationMode presentation_mode);

   public:
    VulkanSwapChain();
    ~VulkanSwapChain();

    VulkanSwapChain(const VulkanSwapChain&)            = delete;
    VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

    void init(EspPresentationMode presentation_mode);
    void terminate();

    inline VkResult acquire_next_image(std::vector<VkSemaphore>& image_available_semaphores)
    {
      return vkAcquireNextImageKHR(VulkanDevice::get_logical_device(),
                                   m_swap_chain,
                                   std::numeric_limits<uint64_t>::max(),
                                   image_available_semaphores[m_current_frame], // must be a not signaled
                                                                                // semaphore
                                   VK_NULL_HANDLE,
                                   &m_image_index);
    }

    inline void go_to_next_frame() { m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT; }

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    inline static VkImage get_current_image()
    {
      return s_instance->m_swap_chain_buffers[s_instance->m_image_index].m_image;
    }

    inline static VkImageView get_current_image_view()
    {
      return s_instance->m_swap_chain_buffers[s_instance->m_image_index].m_image_view;
    }

    static std::unique_ptr<VulkanSwapChain> create(EspPresentationMode presentation_mode);
    static SwapChainSupportDetails query_swap_chain_support(const VkPhysicalDevice& device,
                                                            const VulkanContextData& context_data);
    inline static VkFormat* get_swap_chain_image_format() { return &(s_instance->m_swap_chain_image_format); }
    inline static uint32_t get_current_frame_index() { return s_instance->m_current_frame; }
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_SWAP_CHAIN_HH
