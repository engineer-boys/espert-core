#ifndef ESPERT_PLATFORMS_OPENGLMINIRAPI_OPENGLMINIRCONTEXT_HH_
#define ESPERT_PLATFORMS_OPENGLMINIRAPI_OPENGLMINIRCONTEXT_HH_

#include "Core/MiniRAPI/MiniRContext.hh"
#include "esppch.hh"

// libs
#include "volk.h"

namespace esp
{
  class OpenGLMiniRAPIContext : public MiniRContext
  {
   public:
    OpenGLMiniRAPIContext(const OpenGLMiniRAPIContext& other) = delete;

    OpenGLMiniRAPIContext&
    operator=(const OpenGLMiniRAPIContext& other) = delete;

    virtual ~OpenGLMiniRAPIContext() override;
    OpenGLMiniRAPIContext();

    void init(EspWindow& window);
    void terminate();
    void update();
  };
} // namespace esp

#endif /* ESPERT_PLATFORMS_OPENGLMINIRAPI_OPENGLMINIRCONTEXT_HH_ */