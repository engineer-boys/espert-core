#ifndef ESP_RENDERER_HH
#define ESP_RENDERER_HH

#include "Core/RenderAPI/EspFrameManager.hh"
#include "VulkanSwapChain.hh"

// std
#include <memory>
#include <vector>

namespace esp
{
  class VulkanFrameManager : public EspFrameManager
  {
   private:
    static VulkanFrameManager* s_instance;

    std::unique_ptr<VulkanSwapChain> m_swap_chain;
    std::vector<VkCommandBuffer> m_command_buffers;

    uint32_t m_current_image_index;
    int m_current_frame_index{ 0 };
    bool m_frame_started{ false };

   public:
    static std::unique_ptr<VulkanFrameManager> create();

    VulkanFrameManager(const VulkanFrameManager&)            = delete;
    VulkanFrameManager& operator=(const VulkanFrameManager&) = delete;

    // ------------------------ API ------------------------
    ~VulkanFrameManager() override;

    void init() override;

    void begin_frame() override;
    void end_frame() override;

    void terminate() override;

    void on_window_resized(WindowResizedEvent& e) override;
    // -----------------------------------------------------

    static bool is_frame_in_progress();
    static VkCommandBuffer get_current_command_buffer();
    static int get_current_frame_index();
    static VkRenderPass get_swap_chain_render_pass();
    static std::pair<uint32_t, uint32_t> get_swap_chain_extent();
    static float get_swap_chain_extent_aspect_ratio();

   private:
    VulkanFrameManager();

    void begin_render_pass();
    void end_render_pass();

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
