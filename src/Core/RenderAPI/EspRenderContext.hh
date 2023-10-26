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
    static std::unique_ptr<EspRenderContext> create_and_init(EspWindow& window);

    EspRenderContext()          = default;
    virtual ~EspRenderContext() = default;

    EspRenderContext(const EspRenderContext& other)            = delete;
    EspRenderContext& operator=(const EspRenderContext& other) = delete;

   protected:
    virtual void init(EspWindow& window) = 0;

   public:
    virtual void terminate() = 0;
  };

  void render_context_glfw_hints();
} // namespace esp

#endif // RENDER_API_ESP_RENDER_CONTEXT_H
