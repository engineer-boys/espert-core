#ifndef ESPERT_CORE_MINIRAPI_MINIRCONTEXT_HH_
#define ESPERT_CORE_MINIRAPI_MINIRCONTEXT_HH_

#include "Core/EspWindow.hh"
#include "esppch.hh"

// libs
#include "volk.h"

namespace esp
{
  class MiniRContext
  {
   private:
    static bool s_is_exist;

   public:
    MiniRContext(const MiniRContext& other)            = delete;
    MiniRContext& operator=(const MiniRContext& other) = delete;

    virtual ~MiniRContext() {}
    MiniRContext() {}

    virtual void init(EspWindow& window) = 0;
    virtual void terminate()             = 0;
    virtual void update()                = 0;

    static std::unique_ptr<MiniRContext> create_and_init(EspWindow& window);
  };

  void MiniR_context_glfw_hints();
} // namespace esp

#endif /* ESPERT_CORE_MINIRAPI_MINIRCONTEXT_HH_ */
