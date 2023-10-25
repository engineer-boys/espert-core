#ifndef RENDER_API_ESP_FRAME_SCHEDULER_HH
#define RENDER_API_ESP_FRAME_SCHEDULER_HH

#include "Core/EspWindow.hh"
#include "esppch.hh"

namespace esp
{
  class EspFrameScheduler
  {
   private:
    static bool s_is_exist;

   public:
    static std::unique_ptr<EspFrameScheduler> create_and_init(EspWindow& window);

    EspFrameScheduler()  = default;
    ~EspFrameScheduler() = default;

    EspFrameScheduler(const EspFrameScheduler& other)            = delete;
    EspFrameScheduler& operator=(const EspFrameScheduler& other) = delete;

    virtual void init(EspWindow& window) = 0;
    virtual void begin_frame()           = 0;
    virtual void end_frame()             = 0;

    virtual void begin_swap_chain_render_pass() = 0;
    virtual void end_swap_chain_render_pass()   = 0;
  };
} // namespace esp

#endif // RENDER_API_ESP_FRAME_SCHEDULER_HH
