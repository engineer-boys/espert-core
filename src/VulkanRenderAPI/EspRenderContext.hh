#ifndef RENDERER_RENDER_CONTEXT_H_
#define RENDERER_RENDER_CONTEXT_H_

#include "EspFrameScheduler.hh"

namespace esp
{
  class EspRenderContext
  {
   private:
    static EspRenderContext* s_instance;

    EspDevice m_device;
    EspFrameScheduler m_scheduler;

   public:
    static std::unique_ptr<EspRenderContext>
    create_and_init_vulkan(esp::EspWindow& window);
    inline static EspRenderContext* get_context() { return s_instance; }
    inline static EspDevice& get_device() { return s_instance->m_device; }
    inline static EspFrameScheduler& get_scheduler()
    {
      return s_instance->m_scheduler;
    }

    ~EspRenderContext();

   private:
    EspRenderContext(EspWindow& window);
  };
} // namespace esp

#endif // RENDERER_RENDER_CONTEXT_H_
