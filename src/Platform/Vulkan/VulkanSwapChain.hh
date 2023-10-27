#ifndef RENDERER_ESP_SWAP_CHAIN_HH
#define RENDERER_ESP_SWAP_CHAIN_HH

#include "esppch.hh"

// libs
#include "volk.h"

namespace esp
{
  class VulkanSwapChain
  {
   private:
    VkDevice m_device{};

    std::vector<VkFramebuffer> m_swap_chain_framebuffers;
    VkRenderPass m_render_pass;

    std::vector<VkImage> m_swap_chain_images;
    std::vector<VkImageView> m_swap_chain_image_views;
    VkFormat m_swap_chain_image_format;

    VkImage m_depth_image;
    VkDeviceMemory m_depth_image_memory;
    VkImageView m_depth_image_view;
    VkFormat m_swap_chain_depth_format;

    VkExtent2D m_window_extent;
    VkExtent2D m_swap_chain_extent;

    VkSwapchainKHR m_swap_chain;
    std::shared_ptr<VulkanSwapChain> m_old_swap_chain;

    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence> m_in_flight_fences;
    std::vector<VkFence> m_images_in_flight;
    size_t m_current_frame = 0;

   public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    static bool s_first_initialization;

    static std::unique_ptr<VulkanSwapChain> create(VkExtent2D window_extent);
    static std::unique_ptr<VulkanSwapChain> recreate(VkExtent2D window_extent,
                                                     std::shared_ptr<VulkanSwapChain> previous);

    ~VulkanSwapChain() = default;

    VulkanSwapChain(const VulkanSwapChain&)            = delete;
    VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

    inline VkFramebuffer get_frame_buffer(int index) { return m_swap_chain_framebuffers[index]; }
    inline VkRenderPass get_render_pass() { return m_render_pass; }
    inline VkImageView get_image_view(int index) { return m_swap_chain_image_views[index]; }
    inline size_t get_image_count() { return m_swap_chain_images.size(); }
    inline VkFormat get_image_format() { return m_swap_chain_image_format; }
    inline VkExtent2D get_extent() { return m_swap_chain_extent; }
    inline uint32_t get_extent_width() { return m_swap_chain_extent.width; }
    inline uint32_t get_extent_height() { return m_swap_chain_extent.height; }
    inline float get_extent_aspect_ratio()
    {
      return static_cast<float>(m_swap_chain_extent.width) / static_cast<float>(m_swap_chain_extent.height);
    }

    VkResult acquire_next_image(uint32_t* image_index);
    VkResult submit_command_buffers(const VkCommandBuffer* buffers, uint32_t* image_index);

    inline bool compare_swap_chain_formats(const VulkanSwapChain& swap_chain) const
    {
      return swap_chain.m_swap_chain_depth_format == m_swap_chain_depth_format &&
          swap_chain.m_swap_chain_image_format == m_swap_chain_image_format;
    }

    void terminate();

   private:
    VulkanSwapChain(VkExtent2D window_extent);
    VulkanSwapChain(VkExtent2D window_extent, std::shared_ptr<VulkanSwapChain> previous);

    void init();
    void create_swap_chain();
    void create_image_views();
    void create_depth_resources();
    void create_render_pass();
    void create_framebuffers();
    void create_sync_objects();
  };
} // namespace esp

#endif // RENDERER_ESP_SWAP_CHAIN_HH
