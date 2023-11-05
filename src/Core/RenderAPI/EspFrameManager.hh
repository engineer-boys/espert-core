#ifndef RENDER_API_ESP_FRAME_SCHEDULER_HH
#define RENDER_API_ESP_FRAME_SCHEDULER_HH

#include "Core/EspWindow.hh"
#include "Core/Events/WindowEvent.hh"

#include "esppch.hh"

namespace esp
{
  class EspFrameManager
  {
   private:
    static bool s_is_exist;

   protected:
    struct DepthStencilInfo
    {
      float m_depth;
      uint32_t m_stencil;
    };

    uint32_t m_window_width;
    uint32_t m_window_height;

    DepthStencilInfo m_depth_stencil{ 1.f, 0 };
    glm::vec4 m_clear_color;

   public:
    static std::unique_ptr<EspFrameManager> create_and_init(EspWindow& window,
                                                            glm::vec4 clear_color = { .1f, .1f, .1f, 1.f });

    EspFrameManager()          = default;
    virtual ~EspFrameManager() = default;

    EspFrameManager(const EspFrameManager& other)            = delete;
    EspFrameManager& operator=(const EspFrameManager& other) = delete;

   protected:
    virtual void init() = 0;

   public:
    virtual void begin_frame() = 0;
    virtual void end_frame()   = 0;

    virtual void terminate() = 0;

    virtual void on_window_resized(WindowResizedEvent& e);

    static std::pair<uint32_t, uint32_t> get_swap_chain_extent();
    static float get_swap_chain_extent_aspect_ratio();

    void set_depth_stencil(float depth, uint32_t stencil);
    void set_clear_color(glm::vec3 color);
    void set_clear_color(glm::vec4 color);
  };
} // namespace esp

#endif // RENDER_API_ESP_FRAME_SCHEDULER_HH
