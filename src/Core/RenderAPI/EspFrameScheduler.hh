#ifndef RENDER_API_ESP_FRAME_SCHEDULER_HH
#define RENDER_API_ESP_FRAME_SCHEDULER_HH

#include "Core/EspWindow.hh"
#include "Core/Events/WindowEvent.hh"

#include "esppch.hh"

namespace esp
{
  class EspFrameScheduler
  {
   private:
    static bool s_is_exist;

   protected:
    struct DepthStencilInfo
    {
      float m_depth;
      uint32_t m_stencil;
    };

    unsigned int m_window_width;
    unsigned int m_window_height;

    DepthStencilInfo m_depth_stencil{ 1.f, 0 };
    glm::vec4 m_clear_color{ .1f, .1f, .1f, 1.f };

   public:
    static std::unique_ptr<EspFrameScheduler> create_and_init(EspWindow& window);

    EspFrameScheduler()          = default;
    virtual ~EspFrameScheduler() = default;

    EspFrameScheduler(const EspFrameScheduler& other)            = delete;
    EspFrameScheduler& operator=(const EspFrameScheduler& other) = delete;

    virtual void init()        = 0;
    virtual void begin_frame() = 0;
    virtual void end_frame()   = 0;

    virtual void begin_render_pass() = 0;
    virtual void end_render_pass()   = 0;

    virtual void terminate() = 0;

    virtual void on_window_resized(WindowResizedEvent& e);

    void set_depth_stencil(float depth, uint32_t stencil);
    void set_clear_color(glm::vec3 color);
    void set_clear_color(glm::vec4 color);
  };
} // namespace esp

#endif // RENDER_API_ESP_FRAME_SCHEDULER_HH
