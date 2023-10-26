#ifndef ESP_RENDERER_HH
#define ESP_RENDERER_HH

#include "Core/RenderAPI/EspFrameScheduler.hh"
#include "VulkanSwapChain.hh"

// std
#include <memory>
#include <vector>

namespace esp
{
  class VulkanFrameScheduler : public EspFrameScheduler
  {
   private:
    std::unique_ptr<VulkanSwapChain> m_swap_chain;
    std::vector<VkCommandBuffer> m_command_buffers;

    uint32_t m_current_image_index;
    int m_current_frame_index{ 0 };
    bool m_frame_started{ false };

   public:
    static std::unique_ptr<VulkanFrameScheduler> create();

    VulkanFrameScheduler(const VulkanFrameScheduler&)            = delete;
    VulkanFrameScheduler& operator=(const VulkanFrameScheduler&) = delete;

    ~VulkanFrameScheduler() = default;

    void init() override;

    void begin_frame() override;
    void end_frame() override;

    void begin_render_pass() override;
    void end_render_pass() override;

    void terminate() override;

    void on_window_resized(WindowResizedEvent& e) override;

    inline bool is_frame_in_progress() const { return m_frame_started; };

    inline VkCommandBuffer get_current_command_buffer() const
    {
      ESP_ASSERT(m_frame_started, "Cannot get command buffer if frame hasn't started")
      return m_command_buffers[m_current_frame_index];
    }

    inline int get_current_frame_index()
    {
      ESP_ASSERT(m_frame_started, "Cannot get frame index if frame hasn't started")
      return m_current_frame_index;
    }

    inline VkRenderPass get_swap_chain_render_pass() const { return m_swap_chain->get_render_pass(); };
    inline float get_aspect_ratio() const { return m_swap_chain->get_swap_chain_extent_aspect_ratio(); };

   private:
    VulkanFrameScheduler() = default;

    void create_command_buffers();
    void free_command_buffers();
    void recreate_swap_chain();

    std::array<VkClearValue, 2> get_clear_values();
    void set_viewport();
    void set_scissors();

    VkExtent2D get_window_extent();
  };
} // namespace esp

#endif // ESP_RENDERER_HH
