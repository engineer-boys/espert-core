#ifndef ESPERT_PLATFORMS_GLFWSUPPORT_GLFWCONTEXT_HH_
#define ESPERT_PLATFORMS_GLFWSUPPORT_GLFWCONTEXT_HH_

#include "Core/Events/Event.hh"
#include "esppch.hh"

// libs
#include "GLFW/glfw3.h"

namespace esp
{
  class GLFWContext
  {
   private:
    static bool s_is_exist;

    GLFWContext();

   public:
    ~GLFWContext();

    static std::unique_ptr<GLFWContext> create();
  };
} // namespace esp

#endif // ESPERT_PLATFORMS_GLFWSUPPORT_GLFWCONTEXT_HH_