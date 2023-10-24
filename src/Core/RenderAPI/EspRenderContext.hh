#ifndef RENDER_API_ESP_RENDER_CONTEXT_H
#define RENDER_API_ESP_RENDER_CONTEXT_H

#include "Core/EspWindow.hh"
#include "esppch.hh"

namespace esp
{
  class EspRenderContext
  {
   private:
    static bool s_is_exist;

   public:
    EspRenderContext(const EspRenderContext& other)            = delete;
    EspRenderContext& operator=(const EspRenderContext& other) = delete;

    virtual ~EspRenderContext() {}
    EspRenderContext() {}

    virtual void init(EspWindow& window) = 0;
    virtual void terminate()             = 0;
    virtual void update()                = 0;

    static std::unique_ptr<EspRenderContext> create_and_init(EspWindow& window);
  };

  void render_context_glfw_hints();
} // namespace esp

#endif // RENDER_API_ESP_RENDER_CONTEXT_H
