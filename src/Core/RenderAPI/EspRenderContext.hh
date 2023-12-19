#ifndef RENDER_API_ESP_RENDER_CONTEXT_H
#define RENDER_API_ESP_RENDER_CONTEXT_H

#include "Core/EspWindow.hh"
#include "esppch.hh"

namespace esp
{
  class EspRenderContext
  {
    /* -------------------------- METHODS ---------------------------------- */
   public:
    EspRenderContext()          = default;
    virtual ~EspRenderContext() = default;

    EspRenderContext(const EspRenderContext& other)            = delete;
    EspRenderContext& operator=(const EspRenderContext& other) = delete;

    virtual void init(EspWindow& window) = 0;
    virtual void terminate()             = 0;

    /* -------------------------- STATIC METHODS --------------------------- */
   public:
    static std::unique_ptr<EspRenderContext> build(EspWindow& window);
  };

  void render_context_glfw_hints();
} // namespace esp

#endif // RENDER_API_ESP_RENDER_CONTEXT_H
