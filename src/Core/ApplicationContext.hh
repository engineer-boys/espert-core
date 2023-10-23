#ifndef ESPERT_CORE_APPLICATIONCONTEXT_HH_
#define ESPERT_CORE_APPLICATIONCONTEXT_HH_

#include "Platforms/GLFWSupport/GLFWContext.hh"
#include "esppch.hh"

namespace esp
{
  class ApplicationContext
  {
   private:
    static bool _s_is_exist;

   private:
    std::unique_ptr<Logger> _m_logger;
    std::unique_ptr<GLFWContext> _m_glfw_context;

    ApplicationContext();

   public:
    ~ApplicationContext();

    static std::unique_ptr<ApplicationContext> create();
  };
} // namespace esp

#endif // ESPERT_CORE_APPLICATIONCONTEXT_HH_
